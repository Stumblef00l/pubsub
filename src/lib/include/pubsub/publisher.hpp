#ifndef _PUBSUB_PUBLISHER_HPP_
#define _PUBSUB_PUBLISHER_HPP_

#include <string>
#include <exception>
#include <memory>

#include "structs.hpp"
#include "subscriber_family_manager.hpp"

namespace pubsub {

// Provides an interface for Publishers
class IPublisher {

    public:
        typedef std::unique_ptr<ISubscriberFamilyManager> ISubscriberFamilyManagerUniquePtr;

        // Implements Publish to control publisher behavior
        virtual void Publish(PubsubMessage message) = 0;

        // Implements getter for the SubscriberFamilyManager instance
        virtual ISubscriberFamilyManager* GetSubscriberFamilyManager() const;

        virtual ~IPublisher() {}

    protected:
        // Constructor
        IPublisher(ISubscriberFamilyManagerUniquePtr subscriber_family_manager);
        
        ISubscriberFamilyManagerUniquePtr subscriber_family_manager_;
};

// Implements a synchronous publisher. Implements the IPublisher interface.
class BasicSynchronousPublisher: public IPublisher {

    public:
        BasicSynchronousPublisher(ISubscriberFamilyManagerUniquePtr subscriber_family_manager);

        // Synchronously publishes the message to all subscribers, configured in the
        // subscriber family specified in the message.
        void Publish(PubsubMessage message) override;
};

// Thrown when SubscriberFamilyManager is null
class NullSubscriberFamilyManagerException: public std::exception {
    static constexpr std::string_view errorMessage = "Referenced SubscriberFamilyManager is not defined";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

} // namespace pubsub

#endif