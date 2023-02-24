#include "pubsub/subscriber.hpp"

#include <utility>

#include "pubsub/structs.hpp"

namespace pubsub {

ISubscriber::ISubscriber(PubsubSubscriberId id)
    : id_(std::move(id)) {}

inline PubsubSubscriberId ISubscriber::GetID() const {
    return id_;
}

} // namespace pubsub