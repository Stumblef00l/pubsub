#include "pubsub/subscriber_family.hpp"

#include <exception>
#include <utility>

#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"
#include "pubsub/structs.hpp"

namespace pubsub {

ISubscriberFamily::ISubscriberFamily(
    const ISubscriberFamily::SubscriberFamilyID id,
    ISubscriberFamily::ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
    ISubscriberSelectionStrategy* selection_strategy)
    : id_(id),
      registration_manager_(std::move(registration_manager)),
      selection_strategy_(std::move(selection_strategy)) {}

BasicSubscriberFamily::BasicSubscriberFamily(
    const SubscriberFamilyID id,
    ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
    ISubscriberSelectionStrategy* selection_strategy)
    : ISubscriberFamily(
        id,
        std::move(registration_manager),
        std::move(selection_strategy)) {}

inline ISubscriberFamily::SubscriberFamilyID
BasicSubscriberFamily::GetID() const noexcept {
    return id_;
}

ISubscriberFamilyRegistrationManager*
BasicSubscriberFamily::GetRegistrationManager() const {
    if (registration_manager_ == nullptr)
        throw NullSubscriberFamilyRegistrationManagerException();
    return registration_manager_.get();
}

ISubscriberSelectionStrategy*
BasicSubscriberFamily::GetSelectionStrategy() const {
    if (selection_strategy_ == nullptr)
        throw NullSubscriberFamilySelectionStrategyException();
    return selection_strategy_;
}

inline void BasicSubscriberFamily::SetSelectionStrategy(ISubscriberSelectionStrategy* strategy) noexcept {
    selection_strategy_ = strategy;
}

void BasicSubscriberFamily::Publish(PubsubMessage message) {
    if (registration_manager_ == nullptr)
        throw NullSubscriberFamilyRegistrationManagerException();
    if (selection_strategy_ == nullptr)
        throw NullSubscriberFamilySelectionStrategyException();

    auto subscriber_list = registration_manager_->GetSubscribers();
    auto selected_subscribers = selection_strategy_->select(subscriber_list);
    for(auto &subscriber : selected_subscribers)
        subscriber->update(message);
}

} // namespace pubsub