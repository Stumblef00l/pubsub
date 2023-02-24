#include "pubsub/subscriber_family_registration_manager.hpp"

#include "pubsub/subscriber.hpp"
#include "pubsub/structs.hpp"

namespace pubsub {

ISubscriberFamilyRegistrationManager::ISubscriberFamilyRegistrationManager() {}

BasicSubscriberFamilyRegistrationManager::BasicSubscriberFamilyRegistrationManager()
    : ISubscriberFamilyRegistrationManager() {}

void ISubscriberFamilyRegistrationManager::RegisterSubscriber(ISubscriber* subscriber) {
    auto id = subscriber->GetID();

    for(auto& registeredSubscriber: subscribers_)
        if (id == registeredSubscriber->GetID())
            throw SubscriberAlreadyExistsException();

    subscribers_.push_back(subscriber);
}

void ISubscriberFamilyRegistrationManager::UnregisterSubscriber(const PubsubSubscriberId& id) {
    for(auto idx = (size_t)0; idx < subscribers_.size(); idx++) {
        if (id == subscribers_[idx]->GetID()) {
            subscribers_.erase(subscribers_.begin() + idx);
            return;
        }
    }

    throw SubscriberNotFoundException();
}

inline std::vector<ISubscriber*> ISubscriberFamilyRegistrationManager::GetSubscribers() const {
    return subscribers_;
}

ISubscriber* ISubscriberFamilyRegistrationManager::GetSubscriber(const PubsubMessageId& id) const {
    for(auto& subscriber: subscribers_)
        if (id == subscriber->GetID())
            return subscriber;

    throw SubscriberNotFoundException();
}

} // namespace pubsub