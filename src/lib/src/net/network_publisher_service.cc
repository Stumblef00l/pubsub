#include "pubsub/net/network_publisher_service.hpp"

#include <unistd.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/reactor.hpp"
#include "pubsub/net/remote_subscriber_connection.hpp"
#include "pubsub/net/socket_utils.hpp"
#include "pubsub/net/wire_format.hpp"
#include "pubsub/publisher.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"

namespace pubsub {
namespace net {

NetworkPublisherService::NetworkPublisherService(
    IThreadSafeSubscriberFamilyManagerUniquePtr subscriber_family_manager,
    std::string bind_host,
    uint16_t bind_port,
    IReactorUniquePtr reactor)
    : IAsyncPublisher(std::move(subscriber_family_manager)),
      reactor_(std::move(reactor)),
      reactor_ptr_(reactor_.get()),
      listen_fd_(socket_utils::CreateListeningSocket(bind_host, bind_port)) {
    reactor_ptr_->WatchReadable(listen_fd_, [this] { OnAcceptable(); });
    reactor_thread_ = std::jthread([this] { reactor_ptr_->Run(); });
}

NetworkPublisherService::~NetworkPublisherService() {
    Stop();
}

void
NetworkPublisherService::Publish(
    PubsubMessage message) {
    auto* family = GetSubscriberFamilyManager()->GetFamily(message.family_id);
    family->Publish(std::move(message));
}

uint16_t
NetworkPublisherService::GetBoundPort() const {
    return socket_utils::GetLocalPort(listen_fd_);
}

void
NetworkPublisherService::Stop() {
    reactor_ptr_->Stop();
    if (reactor_thread_.joinable())
        reactor_thread_.join();

    if (listen_fd_ >= 0) {
        ::close(listen_fd_);
        listen_fd_ = -1;
    }
}

void
NetworkPublisherService::PruneClosedConnections() {
    std::erase_if(pending_connections_, [](const auto& entry) {
        return entry.second->IsClosed();
    });

    std::erase_if(active_connections_, [](const auto& connection) {
        return connection->IsClosed();
    });
}

void
NetworkPublisherService::OnAcceptable() {
    PruneClosedConnections();

    while (true) {
        int fd = socket_utils::TryAccept(listen_fd_);
        if (fd < 0)
            return;

        auto connection = std::make_unique<FramedConnection>(
            fd, reactor_ptr_,
            [this, fd](std::string payload) { HandleHandshake(fd, std::move(payload)); });
        connection->Start();

        pending_connections_.emplace(fd, std::move(connection));
    }
}

void
NetworkPublisherService::HandleHandshake(
    int fd,
    std::string payload) {
    auto it = pending_connections_.find(fd);
    if (it == pending_connections_.end())
        return; // Handshake already handled (or connection already gone); ignore stray bytes.

    wire_format::SubscriptionRequest request;
    try {
        request = wire_format::DecodeSubscriptionRequest(payload);
    } catch (const wire_format::MalformedFrameException&) {
        // Close in place; do not destroy the FramedConnection here, as
        // doing so would free the object whose DrainFrames() call is
        // currently executing this very callback. It is safely pruned on
        // the next OnAcceptable() call.
        it->second->Close();
        return;
    }

    ISubscriberFamily* family = nullptr;
    try {
        family = GetSubscriberFamilyManager()->GetFamily(request.family_id);
    } catch (const SubscriberFamilyNotFoundException&) {
        it->second->Close();
        return;
    }

    // Safe to move now: transferring ownership does not destroy the
    // FramedConnection, unlike letting it fall out of scope would.
    auto connection = std::move(it->second);
    pending_connections_.erase(it);

    auto remote_subscriber = std::make_unique<RemoteSubscriberConnection>(
        request.subscriber_id, std::move(connection));

    try {
        family->GetRegistrationManager()->RegisterSubscriber(remote_subscriber.get());
    } catch (const SubscriberAlreadyExistsException&) {
        remote_subscriber->Close();
        active_connections_.push_back(std::move(remote_subscriber));
        return;
    }

    active_connections_.push_back(std::move(remote_subscriber));
}

} // namespace net
} // namespace pubsub
