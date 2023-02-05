#include <exception>
#include <utility>

#include "pubsub/publisher.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"

pubsub::IPublisher::IPublisher(IPublisher::SubscriberFamilyManagerPtr subscriberFamilyManager)
    : subscriberFamilyManager_(std::move(subscriberFamilyManager)) {}

pubsub::BasicSynchronousPublisher::BasicSynchronousPublisher(IPublisher::SubscriberFamilyManagerPtr subscriberFamilyManager)
    : IPublisher(std::move(subscriberFamilyManager)) {}

ISubscriberFamilyManager* pubsub::BasicSynchronousPublisher::getSubscriberFamilyManager() const {
    auto s = subscriberFamilyManager_.get();
    if (s == nullptr || s == NULL) {
        throw NullSubscriberFamilyManagerException();
    }

    return subscriberFamilyManager_.get();
}

void pubsub::BasicSynchronousPublisher::publish(PubsubMessage message) {
    auto s = getSubscriberFamilyManager();
    auto family = s->getSubscriberFamily(message.familyID);
    family->publish(std::move(message));
}
