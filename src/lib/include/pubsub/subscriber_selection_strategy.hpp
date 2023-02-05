#ifndef _SUBSCRIBER_SELECTION_STRATEGY_HPP_
#define _SUBSCRIBER_SELECTION_STRATEGY_HPP_

#include <vector>

#include "subscriber.hpp"

namespace pubsub {
    class ISubscriberSelectionStrategy {
        public:
            virtual std::vector<ISubscriber*> select(const std::vector<ISubscriber*>& subscribers) = 0;
    };
}

#endif