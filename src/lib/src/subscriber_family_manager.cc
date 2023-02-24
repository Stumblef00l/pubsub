#include "pubsub/subscriber_family_manager.hpp"

#include <exception>
#include <utility>

#include "pubsub/subscriber_family.hpp"

namespace pubsub {

ISubscriberFamilyManager::ISubscriberFamilyManager() {}

BasicSubscriberFamilyManager::BasicSubscriberFamilyManager()
    : ISubscriberFamilyManager() {}

void ISubscriberFamilyManager::CreateSubscriberFamily(
    std::unique_ptr<ISubscriberFamily> subscriber_family) {
    auto id = subscriber_family->GetID();

    for(auto& familyItr: subscriber_family_list_)
        if (id == familyItr->GetID())
            throw SubscriberFamilyAlreadyExistsException();

    subscriber_family_list_.push_back(std::move(subscriber_family));
}

void ISubscriberFamilyManager::DeleteSubscriberFamily(
    const PubsubSubscriberFamilyId& family_id) {
    for(auto idx = (size_t)0; idx < subscriber_family_list_.size(); idx++) {
        if (family_id == subscriber_family_list_[idx]->GetID()) {
            subscriber_family_list_.erase(subscriber_family_list_.begin() + idx);
            return;
        }
    }

    throw SubscriberFamilyNotFoundException();
}

ISubscriberFamily*
ISubscriberFamilyManager::GetSubscriberFamily(
    const PubsubSubscriberFamilyId& family_id) const {
    for(auto idx = (size_t)0; idx < subscriber_family_list_.size(); idx++) {
        if (family_id == subscriber_family_list_[idx]->GetID())
            return subscriber_family_list_[idx].get();
    }

    throw SubscriberFamilyNotFoundException();
    return nullptr;
}

} // namespace pubsub