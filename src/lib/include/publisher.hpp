#ifndef _PUBLISHER_HPP_
#define _PUBLISHER_HPP_

#include <memory>

#include "structs.hpp"
#include "subscriber_family_manager.hpp"

// Provides an interface for Publishers
class IPublisher {
    public:
        typedef std::unique_ptr<ISubscriberFamilyManager> SubscriberFamilyManagerPtr;

        // Implements publish to control publisher behavior
        virtual void publish(PubsubMessage message) = 0;

        // Implements getter for the SubscriberFamilyManager instance
        virtual ISubscriberFamilyManager* getSubscriberFamilyManager() const = 0;

    protected:
        SubscriberFamilyManagerPtr subscriberFamilyManager_;

        // Constructor
        IPublisher(SubscriberFamilyManagerPtr subscriberFamilyManager);
};

#endif