#ifndef _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_HPP_
#define _PUBSUB_TESTING_MOCK_SUBSCRIBER_FAMILY_HPP_

#include "pubsub/subscriber_family.hpp"

#include <utility>

#include "gmock/gmock.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"

namespace pubsub {
namespace testing {

class MockSubscriberFamily: public ISubscriberFamily {
    public:
        MockSubscriberFamily(): ISubscriberFamily("", nullptr, nullptr) {}
        
        MOCK_METHOD(void, Publish, (PubsubMessage message), (override));
        MOCK_METHOD(PubsubSubscriberFamilyId, GetID, (), (const, override)); 
        MOCK_METHOD(ISubscriberFamilyRegistrationManager*, GetRegistrationManager, (), (const, override));
        MOCK_METHOD(ISubscriberSelectionStrategy*, GetSelectionStrategy, (), (const, override));
        MOCK_METHOD(void, SetSelectionStrategy, (ISubscriberSelectionStrategy* strategy), (override));
};

} // namespace testing
} // namespace pubsub
#endif