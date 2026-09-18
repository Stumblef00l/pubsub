#ifndef _PUBSUB_NET_NETWORK_PUBLISHER_SERVICE_HPP_
#define _PUBSUB_NET_NETWORK_PUBLISHER_SERVICE_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/reactor.hpp"
#include "pubsub/net/remote_subscriber_connection.hpp"
#include "pubsub/publisher.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"

namespace pubsub {
namespace net {

// A publisher that also runs a TCP server: remote NetworkSubscriberClients
// can connect to it and subscribe to one of its subscriber families, after
// which Publish() delivers messages to them over the network exactly as
// BasicSynchronousPublisher does for local, in-memory subscribers. All of
// the existing family/registration-manager/selection-strategy machinery is
// reused unchanged; a connected remote client is just another ISubscriber
// as far as that machinery is concerned.
//
// Because network I/O happens on a background thread concurrently with
// application calls to Publish(), the injected family manager (and the
// families/registration managers it holds) must be thread-safe.
class NetworkPublisherService: public IAsyncPublisher {
    public:
        typedef std::unique_ptr<IThreadSafeSubscriberFamilyManager> IThreadSafeSubscriberFamilyManagerUniquePtr;
        typedef std::unique_ptr<IReactor> IReactorUniquePtr;

        // Binds and starts listening immediately. If bind_port is 0, the OS
        // assigns an ephemeral port, retrievable via GetBoundPort().
        NetworkPublisherService(
            IThreadSafeSubscriberFamilyManagerUniquePtr subscriber_family_manager,
            std::string bind_host,
            uint16_t bind_port,
            IReactorUniquePtr reactor = std::make_unique<EpollReactor>());

        ~NetworkPublisherService() override;

        void
        Publish(
            PubsubMessage message) override;

        uint16_t
        GetBoundPort() const;

        // Synchronously stops the network listener and disconnects all
        // remote clients. Safe to call more than once.
        void
        Stop();

    private:
        void
        OnAcceptable();

        void
        HandleHandshake(
            int fd,
            std::string payload);

        void
        PruneClosedConnections();

        IReactorUniquePtr reactor_;
        IReactor* reactor_ptr_;
        int listen_fd_;

        // Both only ever touched from the reactor thread.
        std::map<int, std::unique_ptr<FramedConnection>> pending_connections_;
        std::vector<std::unique_ptr<RemoteSubscriberConnection>> active_connections_;

        std::jthread reactor_thread_;
};

} // namespace net
} // namespace pubsub

#endif
