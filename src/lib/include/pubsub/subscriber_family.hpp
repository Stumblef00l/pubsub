#ifndef _PUBSUB_SUBSCRIBER_FAMILY_HPP_
#define _PUBSUB_SUBSCRIBER_FAMILY_HPP_

#include <memory>

#include "structs.hpp"
#include "subscriber_family_registration_manager.hpp"
#include "subscriber_selection_strategy.hpp"

namespace pubsub {

class ISubscriberFamily {
    public:
        typedef std::unique_ptr<ISubscriberFamilyRegistrationManager> ISubscriberFamilyRegistrationManagerUniquePtr;

        virtual void Publish(PubsubMessage message);
        virtual PubsubSubscriberFamilyId GetID() const; 
        virtual ISubscriberFamilyRegistrationManager* GetRegistrationManager() const;
        virtual ISubscriberSelectionStrategy* GetSelectionStrategy() const;
        virtual void SetSelectionStrategy(ISubscriberSelectionStrategy* strategy);

        virtual ~ISubscriberFamily() {}

    protected:
        ISubscriberFamily(
            PubsubSubscriberFamilyId id,
            ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
            ISubscriberSelectionStrategy* selection_strategy);
    
        const PubsubSubscriberFamilyId id_;
        ISubscriberFamilyRegistrationManagerUniquePtr registration_manager_;
        ISubscriberSelectionStrategy* selection_strategy_;
};

class BasicSubscriberFamily : public ISubscriberFamily {
    public:
        BasicSubscriberFamily(
            const PubsubSubscriberFamilyId id,
            ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
            ISubscriberSelectionStrategy* selection_strategy);
};

// Thrown when SubscriberFamily's registration manager is null
class NullSubscriberFamilyRegistrationManagerException: public std::exception {
    static constexpr std::string_view errorMessage = "Referenced ISubscriberFamilyManagerRegistrationManager is not defined";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

// Thrown when SubscriberFamily's selection strategy is null
class NullSubscriberFamilySelectionStrategyException: public std::exception {
    static constexpr std::string_view errorMessage = "Referenced ISubscriberFamilySelectionStrategy is not defined";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

} // namespace pubsub

#endif