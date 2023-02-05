#ifndef _SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "subscriber_family.hpp"

namespace pubsub {
    class ISubscriberFamilyManager {
        public:
            typedef std::unique_ptr<ISubscriberFamily> SubscriberFamilyManagerPtr;

            virtual void createSubscriberFamily(std::unique_ptr<ISubscriberFamily> subscriberFamily) = 0;
            virtual void deleteSubscriberFamily(std::string familyName) = 0;
            virtual ISubscriberFamily* getSubscriberFamily(std::string familyName) const = 0;

        protected:
            std::vector<SubscriberFamilyManagerPtr> subscriberFamilyList_;
            ISubscriberFamilyManager();
    };
}

#endif