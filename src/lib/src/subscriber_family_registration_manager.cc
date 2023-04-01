#include "pubsub/subscriber_family_registration_manager.hpp"

#include <memory>
#include <mutex>
#include <vector>

#include "pubsub/subscriber.hpp"
#include "pubsub/structs.hpp"

namespace pubsub {

ISubscriberFamilyRegistrationManager::ISubscriberFamilyRegistrationManager() {}

IThreadSafeSubscriberFamilyRegistrationManager::IThreadSafeSubscriberFamilyRegistrationManager() {}

BasicSubscriberFamilyRegistrationManager::BasicSubscriberFamilyRegistrationManager()
    : ISubscriberFamilyRegistrationManager() {}

BasicThreadSafeSubscriberFamilyRegistrationManager::BasicThreadSafeSubscriberFamilyRegistrationManager()
    : IThreadSafeSubscriberFamilyRegistrationManager() {}

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

void BasicThreadSafeSubscriberFamilyRegistrationManager::RegisterSubscriber(ISubscriber* subscriber) {
    std::unique_lock lck { subscribers_mtx_ };
    ISubscriberFamilyRegistrationManager::RegisterSubscriber(subscriber);
}

void BasicThreadSafeSubscriberFamilyRegistrationManager::UnregisterSubscriber(const PubsubSubscriberId& id) {
    std::unique_lock lck { subscribers_mtx_ };
    ISubscriberFamilyRegistrationManager::UnregisterSubscriber(id);
}

std::vector<ISubscriber*> BasicThreadSafeSubscriberFamilyRegistrationManager::GetSubscribers() const {
    std::shared_lock lck { subscribers_mtx_ };
    return ISubscriberFamilyRegistrationManager::GetSubscribers();
}

ISubscriber* BasicThreadSafeSubscriberFamilyRegistrationManager::GetSubscriber(const PubsubMessageId& id) const {
    std::shared_lock lck { subscribers_mtx_ };
    return ISubscriberFamilyRegistrationManager::GetSubscriber(id);
}

} // namespace pubsub