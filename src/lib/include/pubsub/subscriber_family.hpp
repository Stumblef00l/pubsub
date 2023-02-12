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
}

#endif