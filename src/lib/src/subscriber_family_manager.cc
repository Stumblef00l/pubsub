#include <exception>
#include <utility>

#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/subscriber_family.hpp"

pubsub::ISubscriberFamilyManager::ISubscriberFamilyManager() {}

pubsub::BasicSubscriberFamilyManager::BasicSubscriberFamilyManager(): ISubscriberFamilyManager() {}

void pubsub::BasicSubscriberFamilyManager::createSubscriberFamily(
    std::unique_ptr<pubsub::ISubscriberFamily> subscriberFamily) {
    auto id = subscriberFamily->getID();

    for(auto &familyItr: subscriberFamilyList_)
        if (id == familyItr->getID())
            throw pubsub::SubscriberFamilyAlreadyExistsException();

    subscriberFamilyList_.push_back(std::move(subscriberFamily));
}

void pubsub::BasicSubscriberFamilyManager::deleteSubscriberFamily(
    const pubsub::ISubscriberFamily::SubscriberFamilyID& familyID) {
    for(auto idx = (size_t)0; idx < subscriberFamilyList_.size(); idx++) {
        if (familyID == subscriberFamilyList_[idx]->getID()) {
            subscriberFamilyList_.erase(subscriberFamilyList_.begin() + idx);
            return;
        }
    }

    throw pubsub::SubscriberFamilyNotFoundException();
}

pubsub::ISubscriberFamily*
pubsub::BasicSubscriberFamilyManager::getSubscriberFamily(
    const pubsub::ISubscriberFamily::SubscriberFamilyID& familyID) const {
    for(auto idx = (size_t)0; idx < subscriberFamilyList_.size(); idx++) {
        if (familyID == subscriberFamilyList_[idx]->getID())
            return subscriberFamilyList_[idx].get();
    }

    throw pubsub::SubscriberFamilyNotFoundException();
    return nullptr;
}