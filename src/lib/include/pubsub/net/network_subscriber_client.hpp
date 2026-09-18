#ifndef _PUBSUB_NET_NETWORK_SUBSCRIBER_CLIENT_HPP_
#define _PUBSUB_NET_NETWORK_SUBSCRIBER_CLIENT_HPP_

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/reactor.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace net {

// An ISubscriber that lives in a different process from the
// NetworkPublisherService it subscribes to. On construction it
// asynchronously connects to the service, sends a subscription handshake
// for the given family, and thereafter forwards every message it receives
// to a local delegate ISubscriber, which contains the actual application
// logic (mirroring how a plain in-process ISubscriber would be invoked by a
// local subscriber family).
//
// Note: the delegate's Update() is invoked from this client's internal IO
// thread, so it should return quickly.
class NetworkSubscriberClient: public ISubscriber {
    public:
        typedef std::unique_ptr<IReactor> IReactorUniquePtr;

        NetworkSubscriberClient(
            PubsubSubscriberId id,
            PubsubSubscriberFamilyId family_id,
            ISubscriber* delegate,
            std::string host,
            uint16_t port,
            IReactorUniquePtr reactor = std::make_unique<EpollReactor>());

        ~NetworkSubscriberClient() override;

        // Forwards to the delegate, so this object remains substitutable
        // anywhere a plain ISubscriber is expected.
        void
        Update(
            PubsubMessage message) override;

        bool
        IsConnected() const;

        // Synchronously disconnects. Safe to call more than once.
        void
        Stop();

    private:
        void
        OnConnectWritable();

        void
        OnMessageFrame(
            std::string payload);

        ISubscriber* delegate_;
        PubsubSubscriberFamilyId family_id_;

        IReactorUniquePtr reactor_;
        IReactor* reactor_ptr_;
        int fd_;

        // Only ever written from the reactor thread; only read from other
        // threads after Stop() has joined that thread.
        std::unique_ptr<FramedConnection> connection_;

        std::atomic<bool> connected_;

        std::jthread reactor_thread_;
};

} // namespace net
} // namespace pubsub

#endif
