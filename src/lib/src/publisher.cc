#include "pubsub/publisher.hpp"

#include <exception>
#include <utility>

#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"

namespace pubsub {

IPublisher::IPublisher(
    IPublisher::ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : subscriber_family_manager_(std::move(subscriber_family_manager)) {}

BasicSynchronousPublisher::BasicSynchronousPublisher(
    IPublisher::ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : IPublisher(std::move(subscriber_family_manager)) {}

ISubscriberFamilyManager*
IPublisher::GetSubscriberFamilyManager() const {
    auto s = subscriber_family_manager_.get();
    if (s == nullptr || s == NULL)
        throw NullSubscriberFamilyManagerException();

    return s;
}

void BasicSynchronousPublisher::Publish(
    PubsubMessage message) {
    auto s = subscriber_family_manager_.get();
    auto family = s->GetFamily(message.family_id);
    family->Publish(std::move(message));
}

} // namespace pubsub
