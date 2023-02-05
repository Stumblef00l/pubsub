#ifndef _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_

#include <memory>
#include <vector>

#include "subscriber.hpp"

namespace pubsub {
    class ISubscriberFamilyRegistrationManager {
        public:
            virtual void registerSubscriber(ISubscriber* subscriber) = 0;
            virtual void unregisterSubscriber(ISubscriber* subscriber) = 0;
            virtual std::vector<ISubscriber*> getSubscribers() = 0;

        protected:
            std::vector<ISubscriber*> subscribers_;
    };
}

#endif