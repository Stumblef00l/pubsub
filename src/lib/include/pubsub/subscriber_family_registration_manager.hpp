#ifndef _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_
#define _SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_

#include <memory>
#include <vector>

#include "subscriber.hpp"

namespace pubsub {

class ISubscriberFamilyRegistrationManager {
    public:

        virtual void RegisterSubscriber(ISubscriber* subscriber) = 0;
        virtual void UnregisterSubscriber(ISubscriber* subscriber) = 0;
        virtual std::vector<ISubscriber*> GetSubscribers() const = 0;

    protected:
        ISubscriberFamilyRegistrationManager();

        std::vector<ISubscriber*> subscribers_;
};

class BasicSubscriberFamilyRegistrationManager: public ISubscriberFamilyRegistrationManager {
    public:
        BasicSubscriberFamilyRegistrationManager();

        void RegisterSubscriber(ISubscriber* subscriber) override;
        void UnregisterSubscriber(ISubscriber* subscriber) override;
        std::vector<ISubscriber*> GetSubscribers() const override;
};

} // namespace pubsub

#endif