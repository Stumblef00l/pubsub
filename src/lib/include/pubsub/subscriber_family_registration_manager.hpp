#ifndef _PUBSUB_SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_
#define _PUBSUB_SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_

#include <memory>
#include <vector>

#include "structs.hpp"
#include "subscriber.hpp"

namespace pubsub {

class ISubscriberFamilyRegistrationManager {
    public:

        virtual void RegisterSubscriber(ISubscriber* subscriber);
        virtual void UnregisterSubscriber(const PubsubSubscriberId& id);
        virtual std::vector<ISubscriber*> GetSubscribers() const;
        virtual ISubscriber* GetSubscriber(const PubsubSubscriberId& id) const;

        virtual ~ISubscriberFamilyRegistrationManager() {}

    protected:
        ISubscriberFamilyRegistrationManager();

        std::vector<ISubscriber*> subscribers_;
};

class BasicSubscriberFamilyRegistrationManager: public ISubscriberFamilyRegistrationManager {
    public:
        BasicSubscriberFamilyRegistrationManager();
};


// Thrown when trying to register a subscriber that is already registered
class SubscriberAlreadyExistsException: public std::exception {
    static constexpr std::string_view errorMessage = "A Subscriber with provided ID is already registered";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

// Thrown when trying to unregister a subscriber that isn't currently registered
class SubscriberNotFoundException: public std::exception {
    static constexpr std::string_view errorMessage = "A registered subscriber with provided ID was not found.";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

} // namespace pubsub

#endif