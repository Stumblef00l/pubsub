#ifndef _PUBSUB_SUBSCRIBER_SELECTION_STRATEGY_HPP_
#define _PUBSUB_SUBSCRIBER_SELECTION_STRATEGY_HPP_

#include <vector>

#include "subscriber.hpp"

namespace pubsub {

class ISubscriberSelectionStrategy {
    public:
        virtual std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) = 0;

        virtual ~ISubscriberSelectionStrategy() {}
};

class RoundRobinSubscriberSelectionStrategy: public ISubscriberSelectionStrategy {
    public:
        RoundRobinSubscriberSelectionStrategy();

        std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) override;
    
    private:
        size_t current_selected_index_;
};

} // namespace pubsub

#endif