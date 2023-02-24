#include "pubsub/subscriber_family.hpp"

#include <exception>
#include <utility>

#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"
#include "pubsub/structs.hpp"

namespace pubsub {

ISubscriberFamily::ISubscriberFamily(
    PubsubSubscriberFamilyId id,
    ISubscriberFamily::ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
    ISubscriberSelectionStrategy* selection_strategy)
    : id_(std::move(id)),
      registration_manager_(std::move(registration_manager)),
      selection_strategy_(std::move(selection_strategy)) {}

BasicSubscriberFamily::BasicSubscriberFamily(
    const PubsubSubscriberFamilyId id,
    ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
    ISubscriberSelectionStrategy* selection_strategy)
    : ISubscriberFamily(
        id,
        std::move(registration_manager),
        std::move(selection_strategy)) {}

inline PubsubSubscriberFamilyId
ISubscriberFamily::GetID() const {
    return id_;
}

ISubscriberFamilyRegistrationManager*
ISubscriberFamily::GetRegistrationManager() const {
    if (registration_manager_ == nullptr)
        throw NullSubscriberFamilyRegistrationManagerException();
    return registration_manager_.get();
}

ISubscriberSelectionStrategy*
ISubscriberFamily::GetSelectionStrategy() const {
    if (selection_strategy_ == nullptr)
        throw NullSubscriberFamilySelectionStrategyException();
    return selection_strategy_;
}

inline void ISubscriberFamily::SetSelectionStrategy(ISubscriberSelectionStrategy* strategy) {
    selection_strategy_ = strategy;
}

void ISubscriberFamily::Publish(PubsubMessage message) {
    if (registration_manager_ == nullptr)
        throw NullSubscriberFamilyRegistrationManagerException();
    if (selection_strategy_ == nullptr)
        throw NullSubscriberFamilySelectionStrategyException();

    auto subscriber_list = registration_manager_->GetSubscribers();
    auto selected_subscribers = selection_strategy_->Select(subscriber_list);
    for(auto& subscriber : selected_subscribers)
        subscriber->Update(message);
}

} // namespace pubsub