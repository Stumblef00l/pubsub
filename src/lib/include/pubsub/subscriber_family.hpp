#ifndef _SUBSCRIBER_FAMILY_HPP_
#define _SUBSCRIBER_FAMILY_HPP_

#include <memory>

#include "subscriber_family_registration_manager.hpp"
#include "subscriber_selection_strategy.hpp"
#include "structs.hpp"

namespace pubsub {

class ISubscriberFamily {
    public:
        typedef std::unique_ptr<ISubscriberFamilyRegistrationManager> ISubscriberFamilyRegistrationManagerUniquePtr;
        typedef std::string SubscriberFamilyID;

        virtual void Publish(PubsubMessage message) = 0;
        virtual SubscriberFamilyID GetID() const = 0; 
        virtual ISubscriberFamilyRegistrationManager* GetRegistrationManager() const = 0;
        virtual ISubscriberSelectionStrategy* GetSelectionStrategy() const = 0;
        virtual void SetSelectionStrategy(ISubscriberSelectionStrategy* strategy) = 0;

    protected:
        ISubscriberFamily(
            const SubscriberFamilyID id,
            ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
            ISubscriberSelectionStrategy* selection_strategy);
    
        const SubscriberFamilyID id_;
        ISubscriberFamilyRegistrationManagerUniquePtr registration_manager_;
        ISubscriberSelectionStrategy* selection_strategy_;
};

class BasicSubscriberFamily : public ISubscriberFamily {
    public:
        BasicSubscriberFamily(
            const SubscriberFamilyID id,
            ISubscriberFamilyRegistrationManagerUniquePtr registration_manager,
            ISubscriberSelectionStrategy* selection_strategy);
        
        void Publish(PubsubMessage message) override;
        inline SubscriberFamilyID GetID() const noexcept override; 
        ISubscriberFamilyRegistrationManager* GetRegistrationManager() const override;
        ISubscriberSelectionStrategy* GetSelectionStrategy() const override;
        inline void SetSelectionStrategy(ISubscriberSelectionStrategy* strategy) noexcept override;
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