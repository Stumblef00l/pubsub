#ifndef _PUBLISHER_HPP_
#define _PUBLISHER_HPP_

#include <string>
#include <exception>
#include <memory>

#include "structs.hpp"
#include "subscriber_family_manager.hpp"

namespace pubsub {
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

    // Implements a synchronous publisher. Implements the IPublisher interface.
    class BasicSynchronousPublisher: public IPublisher {

        public:
            BasicSynchronousPublisher(SubscriberFamilyManagerPtr subscriberFamilyManager);

            // Synchronously publishes the message to all subscribers, configured in the
            // subscriber family specified in the message.
            void publish(PubsubMessage message) override;

            // Returns the associated SubscriberFamilyManager
            ISubscriberFamilyManager* getSubscriberFamilyManager() const override;
    };

    // Thrown when SubscriberFamilyManager is null
    class NullSubscriberFamilyManagerException: public std::exception {
        static constexpr std::string_view ERROR_MESSAGE = "Referenced SubscriberFamilyManager is not defined";
        
        public:
            // Displays the error message
            inline const char*
            what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
                return ERROR_MESSAGE.data();
            };
    };
};

#endif