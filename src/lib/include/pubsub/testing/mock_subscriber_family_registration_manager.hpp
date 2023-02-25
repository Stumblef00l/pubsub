#ifndef _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_
#define _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_REGISTRATION_MANAGER_HPP_

#include "pubsub/subscriber_family_registration_manager.hpp"

#include "gmock/gmock.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace testing {

class MockSubscriberFamilyRegistrationManager: public ISubscriberFamilyRegistrationManager {
    public:
        MockSubscriberFamilyRegistrationManager() {}
        
        MOCK_METHOD(void, RegisterSubscriber, (ISubscriber* subscriber), (override));
        MOCK_METHOD(void, UnregisterSubscriber, (const PubsubSubscriberId& id), (override));
        MOCK_METHOD(std::vector<ISubscriber*>, GetSubscribers, (), (const, override));
        MOCK_METHOD(ISubscriber*, GetSubscriber, (const PubsubSubscriberId& id), (const, override));
};

} // namespace testing    
} // namespace pubsub

#endif