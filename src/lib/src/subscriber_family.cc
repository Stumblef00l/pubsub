#include <exception>
#include <utility>

#include "pubsub/subscriber_family.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"
#include "pubsub/structs.hpp"

pubsub::ISubscriberFamily::ISubscriberFamily(
    const pubsub::ISubscriberFamily::SubscriberFamilyID id,
    pubsub::ISubscriberFamily::RegistrationManagerPtr registrationManager,
    pubsub::ISubscriberSelectionStrategy* selectionStrategy)
    : id_(id),
      registrationManager_(std::move(registrationManager)),
      selectionStrategy_(std::move(selectionStrategy)) {}

pubsub::BasicSubscriberFamily::BasicSubscriberFamily(
    const SubscriberFamilyID id,
    RegistrationManagerPtr registrationManager,
    ISubscriberSelectionStrategy* selectionStrategy)
    : ISubscriberFamily(
        id,
        std::move(registrationManager),
        std::move(selectionStrategy)) {}

inline pubsub::ISubscriberFamily::SubscriberFamilyID
pubsub::BasicSubscriberFamily::getID() const noexcept {
    return id_;
}

pubsub::ISubscriberFamilyRegistrationManager*
pubsub::BasicSubscriberFamily::getRegistrationManager() const {
    if (registrationManager_ == nullptr)
        throw pubsub::NullSubscriberFamilyRegistrationManager();
    return registrationManager_.get();
}

pubsub::ISubscriberSelectionStrategy*
pubsub::BasicSubscriberFamily::getSelectionStrategy() const {
    if (selectionStrategy_ == nullptr)
        throw pubsub::NullSubscriberFamilySelectionStrategy();
    return selectionStrategy_;
}

inline void pubsub::BasicSubscriberFamily::setSelectionStrategy(ISubscriberSelectionStrategy* strategy) noexcept {
    selectionStrategy_ = strategy;
}

void pubsub::BasicSubscriberFamily::publish(pubsub::PubsubMessage message) {
    if (registrationManager_ == nullptr)
        throw pubsub::NullSubscriberFamilyRegistrationManager();
    if (selectionStrategy_ == nullptr)
        throw pubsub::NullSubscriberFamilySelectionStrategy();

    auto subscriberList = registrationManager_->getSubscribers();
    auto selectedSubscribers = selectionStrategy_->select(subscriberList);
    for(auto &subscriber : selectedSubscribers)
        subscriber->update(message);
}