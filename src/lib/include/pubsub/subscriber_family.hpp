#ifndef _SUBSCRIBER_FAMILY_HPP_
#define _SUBSCRIBER_FAMILY_HPP_

#include <memory>

#include "subscriber_family_registration_manager.hpp"
#include "subscriber_selection_strategy.hpp"
#include "structs.hpp"

namespace pubsub {
    class ISubscriberFamily {
        public:
            typedef std::unique_ptr<ISubscriberFamilyRegistrationManager> RegistrationManagerPtr;
            typedef std::string SubscriberFamilyID;

            virtual void publish(PubsubMessage message) = 0;
            virtual SubscriberFamilyID getID() const = 0; 
            virtual ISubscriberFamilyRegistrationManager* getRegistrationManager() const = 0;
            virtual ISubscriberSelectionStrategy* getSelectionStrategy() const = 0;
            virtual void setSelectionStrategy(ISubscriberSelectionStrategy* strategy) = 0;

        protected:
            const SubscriberFamilyID id_;

            RegistrationManagerPtr registrationManager_;
            ISubscriberSelectionStrategy* selectionStrategy_;

            ISubscriberFamily(
                const SubscriberFamilyID id,
                RegistrationManagerPtr registrationManager,
                ISubscriberSelectionStrategy* selectionStrategy);
    };

    class BasicSubscriberFamily : public ISubscriberFamily {
        public:
            BasicSubscriberFamily(
                const SubscriberFamilyID id,
                RegistrationManagerPtr registrationManager,
                ISubscriberSelectionStrategy* selectionStrategy);
            
            void publish(PubsubMessage message) override;
            inline SubscriberFamilyID getID() const noexcept override; 
            ISubscriberFamilyRegistrationManager* getRegistrationManager() const override;
            ISubscriberSelectionStrategy* getSelectionStrategy() const override;
            inline void setSelectionStrategy(ISubscriberSelectionStrategy* strategy) noexcept override;
    };

    // Thrown when SubscriberFamily's registration manager is null
    class NullSubscriberFamilyRegistrationManager: public std::exception {
        static constexpr std::string_view ERROR_MESSAGE = "Referenced ISubscriberFamilyManagerRegistrationManager is not defined";
        
        public:
            // Displays the error message
            inline const char*
            what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return ERROR_MESSAGE.data();
            };
    };

    // Thrown when SubscriberFamily's selection strategy is null
    class NullSubscriberFamilySelectionStrategy: public std::exception {
        static constexpr std::string_view ERROR_MESSAGE = "Referenced ISubscriberFamilySelectionStrategy is not defined";
        
        public:
            // Displays the error message
            inline const char*
            what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return ERROR_MESSAGE.data();
            };
    };
}

#endif