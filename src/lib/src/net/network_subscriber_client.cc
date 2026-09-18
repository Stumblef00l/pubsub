#include "pubsub/net/network_subscriber_client.hpp"

#include <unistd.h>

#include <memory>
#include <string>
#include <utility>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/reactor.hpp"
#include "pubsub/net/socket_utils.hpp"
#include "pubsub/net/wire_format.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace net {

NetworkSubscriberClient::NetworkSubscriberClient(
    PubsubSubscriberId id,
    PubsubSubscriberFamilyId family_id,
    ISubscriber* delegate,
    std::string host,
    uint16_t port,
    IReactorUniquePtr reactor)
    : ISubscriber(std::move(id)),
      delegate_(delegate),
      family_id_(std::move(family_id)),
      reactor_(std::move(reactor)),
      reactor_ptr_(reactor_.get()),
      fd_(socket_utils::StartConnect(host, port)),
      connected_(false) {
    reactor_ptr_->WatchWritable(fd_, [this] { OnConnectWritable(); });
    reactor_thread_ = std::jthread([this] { reactor_ptr_->Run(); });
}

NetworkSubscriberClient::~NetworkSubscriberClient() {
    Stop();
}

void
NetworkSubscriberClient::Update(
    PubsubMessage message) {
    delegate_->Update(std::move(message));
}

bool
NetworkSubscriberClient::IsConnected() const {
    return connected_.load();
}

void
NetworkSubscriberClient::Stop() {
    reactor_ptr_->Stop();
    if (reactor_thread_.joinable())
        reactor_thread_.join();

    // By this point the reactor thread has fully exited, so it is now safe
    // for this (the owning) thread to touch connection_/fd_ directly.
    if (connection_) {
        connection_->Close();
    } else if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

void
NetworkSubscriberClient::OnConnectWritable() {
    reactor_ptr_->StopWatchingWritable(fd_);

    try {
        socket_utils::CheckConnectCompleted(fd_);
    } catch (const socket_utils::SocketException&) {
        ::close(fd_);
        fd_ = -1;
        return;
    }

    connection_ = std::make_unique<FramedConnection>(
        fd_, reactor_ptr_,
        [this](std::string payload) { OnMessageFrame(std::move(payload)); });
    connection_->Start();
    connection_->Send(wire_format::EncodeSubscriptionRequest({GetID(), family_id_}));

    connected_.store(true);
}

void
NetworkSubscriberClient::OnMessageFrame(
    std::string payload) {
    PubsubMessage message;
    try {
        message = wire_format::DecodeMessage(payload);
    } catch (const wire_format::MalformedFrameException&) {
        return;
    }

    delegate_->Update(std::move(message));
}

} // namespace net
} // namespace pubsub
