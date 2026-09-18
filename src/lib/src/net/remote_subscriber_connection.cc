#include "pubsub/net/remote_subscriber_connection.hpp"

#include <memory>
#include <utility>

#include "pubsub/net/framed_connection.hpp"
#include "pubsub/net/wire_format.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace net {

RemoteSubscriberConnection::RemoteSubscriberConnection(
    PubsubSubscriberId id,
    std::unique_ptr<FramedConnection> connection)
    : ISubscriber(std::move(id)),
      connection_(std::move(connection)) {}

void
RemoteSubscriberConnection::Update(
    PubsubMessage message) {
    connection_->Send(wire_format::EncodeMessage(message));
}

void
RemoteSubscriberConnection::Close() {
    connection_->Close();
}

bool
RemoteSubscriberConnection::IsClosed() const {
    return connection_->IsClosed();
}

} // namespace net
} // namespace pubsub
