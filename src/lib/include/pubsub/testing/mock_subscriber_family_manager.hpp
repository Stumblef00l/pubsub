#ifndef _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_MANAGER_HPP_
#define _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_MANAGER_HPP_

#include "pubsub/subscriber_family_manager.hpp"

#include "gmock/gmock.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family.hpp"

namespace pubsub {
namespace testing {

class MockSubscriberFamilyManager: public ISubscriberFamilyManager {
    public:
        MockSubscriberFamilyManager(): ISubscriberFamilyManager() {};

        MOCK_METHOD(void, CreateFamily, (ISubscriberFamilyUniquePtr subscriber_family), (override));
        MOCK_METHOD(void, DeleteFamily, (const PubsubSubscriberFamilyId& family_id), (override));
        MOCK_METHOD(ISubscriberFamily*, GetFamily, (const PubsubSubscriberFamilyId& family_id), (const, override));
};

} // namespace testing
} // namespace pubsub

#endif