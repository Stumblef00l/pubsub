#include "pubsub/subscriber.hpp"

namespace pubsub {

ISubscriber::ISubscriber(const ISubscriber::SubscriberID id)
    : id_(id) {}

} // namespace pubsub