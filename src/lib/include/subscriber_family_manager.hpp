#ifndef _SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_MANAGER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "subscriber_family.hpp"

class ISubscriberFamilyManager {
    public:
        typedef std::unique_ptr<ISubscriberFamily> ISFPtr;

        virtual void createSubscriberFamily(std::unique_ptr<ISubscriberFamily> subscriberFamily) = 0;
        virtual void deleteSubscriberFamily(std::string familyName) = 0;
        virtual ISubscriberFamily* getSubscriberFamily(std::string familyName) = 0;

    protected:
        std::vector<ISubscriberFamilyManager::ISFPtr> sfList_;
        ISubscriberFamilyManager();
};

#endif