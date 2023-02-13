#include "pubsub/subscriber_family_registration_manager.hpp"

namespace pubsub {

ISubscriberFamilyRegistrationManager::ISubscriberFamilyRegistrationManager() {}

BasicSubscriberFamilyRegistrationManager::BasicSubscriberFamilyRegistrationManager()
    : ISubscriberFamilyRegistrationManager() {}

void BasicSubscriberFamilyRegistrationManager::RegisterSubscriber(ISubscriber* subscriber) {
    auto id = subscriber->getID();

    for(auto& registeredSubscriber: subscribers_)
        if (id == registeredSubscriber->getID())
            throw SubscriberAlreadyExistsException();

    subscribers_.push_back(subscriber);
}

void BasicSubscriberFamilyRegistrationManager::UnregisterSubscriber(ISubscriber::SubscriberID id) {
    for(auto idx = (size_t)0; idx < subscribers_.size(); idx++) {
        if (id == subscribers_[idx]->getID()) {
            subscribers_.erase(subscribers_.begin() + idx);
            return;
        }
    }

    throw SubscriberNotFoundException();
}

inline std::vector<ISubscriber*> BasicSubscriberFamilyRegistrationManager::GetSubscribers() const {
    return subscribers_;
}

ISubscriber* BasicSubscriberFamilyRegistrationManager::GetSubscriber(ISubscriber::SubscriberID id) const {
    for(auto& subscriber: subscribers_)
        if (id == subscriber->getID())
            return subscriber;

    throw SubscriberNotFoundException();
}

} // namespace pubsub