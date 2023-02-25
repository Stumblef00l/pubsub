#ifndef _PUBSUB_TESTING_MOCK_SUBSCRIBER_HPP_
#define _PUBSUB_TESTING_MOCK_SUBSCRIBER_HPP_

#include "pubsub/subscriber.hpp"

#include <utility>

#include "gmock/gmock.h"
#include "pubsub/structs.hpp"

namespace pubsub {
namespace testing {

class MockSubscriber: public ISubscriber {
    public:

        MockSubscriber(PubsubSubscriberId id): ISubscriber(std::move(id)) {}

        MOCK_METHOD(PubsubSubscriberId, GetID, (), (const, override)); 
        MOCK_METHOD(void, Update, (PubsubMessage message), (override));
};

} // namespace testing
} // namespace pubsub

#endif