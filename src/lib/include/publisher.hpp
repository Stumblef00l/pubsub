#ifndef _PUBLISHER_HPP_
#define _PUBLISHER_HPP_

#include "subscriber_family_manager.hpp"
#include "structs.hpp"

// Provides an interface for Publishers
class IPublisher {
    protected:
        ISubscriberFamilyManager *sfmgr_;

        // Constructor
        IPublisher(ISubscriberFamilyManager *sfmgr);
    public:
        // Implement publish to control publisher behavior
        virtual void publish(PubsubMessage msg) = 0;

        // Implement getter for the SubscriberFamilyManager instance
        virtual ISubscriberFamilyManager* getSubscriberFamilyManager() = 0;
};

#endif