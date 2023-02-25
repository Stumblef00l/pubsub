#ifndef _PUBSUB_TESTING_MOCK_SUBSCRIBER_SELECTION_STRATEGY_HPP_
#define _PUBSUB_TESTING_MOCK_SUBSCRIBER_SELECTION_STRATEGY_HPP_

#include "pubsub/subscriber_selection_strategy.hpp"

#include <vector>

#include "gmock/gmock.h"
#include "pubsub/subscriber.hpp"

namespace pubsub {
namespace testing {

class MockSubscriberSelectionStrategy: public ISubscriberSelectionStrategy {
    public:
        MockSubscriberSelectionStrategy() {}
        
        MOCK_METHOD(std::vector<ISubscriber*>, Select, (const std::vector<ISubscriber*>& subscribers), (override));
};

} // namespace testing
} // namespace pubsub

#endif