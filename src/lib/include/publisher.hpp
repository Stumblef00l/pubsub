#ifndef _PUBLISHER_HPP_
#define _PUBLISHER_HPP_

#include <memory>

#include "structs.hpp"
#include "subscriber_family_manager.hpp"

// Provides an interface for Publishers
class IPublisher {
    public:
        // Implement publish to control publisher behavior
        virtual void publish(PubsubMessage msg) = 0;

        // Implement getter for the SubscriberFamilyManager instance
        virtual ISubscriberFamilyManager* getSubscriberFamilyManager() = 0;

    protected:
        std::unique_ptr<ISubscriberFamilyManager> sfmgr_;

        // Constructor
        IPublisher(std::unique_ptr<ISubscriberFamilyManager> sfmgr);
};

#endif