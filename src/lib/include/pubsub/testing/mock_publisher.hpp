#ifndef _PUBSUB_TESTING_MOCK_PUBLISHER_HPP_
#define _PUBSUB_TESTING_MOCK_PUBLISHER_HPP_

#include "pubsub/publisher.hpp"

#include <utility>

#include "gmock/gmock.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"

namespace pubsub {
namespace testing {

class MockPublisher: public IPublisher {
    MockPublisher(ISubscriberFamilyManagerUniquePtr family_manager)
    : IPublisher(std::move(family_manager)) {}
    
    MOCK_METHOD(void, Publish, (PubsubMessage message), (override));
    MOCK_METHOD(ISubscriberFamilyManager*, GetSubscriberFamilyManager, (), (const, override));
};

} // namespace testing
} // namespace pubsub

#endif