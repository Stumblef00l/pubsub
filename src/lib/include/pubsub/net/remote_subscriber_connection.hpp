#ifndef _PUBSUB_NET_REMOTE_SUBSCRIBER_CONNECTION_HPP_
#define _PUBSUB_NET_REMOTE_SUBSCRIBER_CONNECTION_HPP_

#include <memory>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace net {

// An ISubscriber that represents a remote NetworkSubscriberClient connected
// to a NetworkPublisherService. Update() forwards the message across the
// network connection instead of invoking any local business logic.
class RemoteSubscriberConnection: public ISubscriber {
    public:
        RemoteSubscriberConnection(
            PubsubSubscriberId id,
            std::unique_ptr<FramedConnection> connection);

        void
        Update(
            PubsubMessage message) override;

        // Closes the underlying connection. Publishing to a closed
        // connection is a harmless no-op.
        void
        Close();

        bool
        IsClosed() const;

    private:
        std::unique_ptr<FramedConnection> connection_;
};

} // namespace net
} // namespace pubsub

#endif
