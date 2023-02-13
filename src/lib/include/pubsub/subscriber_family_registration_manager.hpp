#ifndef _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_

#include <memory>
#include <vector>

#include "subscriber.hpp"

namespace pubsub {

class ISubscriberFamilyRegistrationManager {
    public:

        virtual void RegisterSubscriber(ISubscriber* subscriber) = 0;
        virtual void UnregisterSubscriber(ISubscriber::SubscriberID id) = 0;
        virtual std::vector<ISubscriber*> GetSubscribers() const = 0;
        virtual ISubscriber* GetSubscriber(ISubscriber::SubscriberID id) const = 0;

    protected:
        ISubscriberFamilyRegistrationManager();

        std::vector<ISubscriber*> subscribers_;
};

class BasicSubscriberFamilyRegistrationManager: public ISubscriberFamilyRegistrationManager {
    public:
        BasicSubscriberFamilyRegistrationManager();

        void RegisterSubscriber(ISubscriber* subscriber) override;
        void UnregisterSubscriber(ISubscriber::SubscriberID id) override;
        std::vector<ISubscriber*> GetSubscribers() const override;
        ISubscriber* GetSubscriber(ISubscriber::SubscriberID id) const override;
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