#include "pubsub/publisher.hpp"

#include <exception>
#include <utility>

#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"

namespace pubsub {

IPublisher::IPublisher(
    IPublisher::ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : subscriber_family_manager_(std::move(subscriber_family_manager)) {}

IPublisherDecorator::IPublisherDecorator(std::unique_ptr<IPublisher> publisher)
    : IPublisher(nullptr), // Don't use the inherited data members from the IPublisher interface. Instead, use publisher_.
      publisher_(std::move(publisher)) {}

ISubscriberFamilyManager*
IPublisherDecorator::GetSubscriberFamilyManager() const {
    return publisher_->GetSubscriberFamilyManager();
}

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
