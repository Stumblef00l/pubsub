#include "pubsub/subscriber_selection_strategy.hpp"

#include "pubsub/subscriber.hpp"

namespace pubsub {

RoundRobinSubscriberSelectionStrategy::RoundRobinSubscriberSelectionStrategy()
    : current_selected_index_(0) {}

std::vector<ISubscriber*>
RoundRobinSubscriberSelectionStrategy::select(const std::vector<ISubscriber*>& subscribers) {
    if(subscribers.empty())
        return {};
    
    if(current_selected_index_ >= subscribers.size())
        current_selected_index_ = 0;
    
    auto selected_subscribers_list = std::vector<ISubscriber*>{ subscribers[0] };
    return selected_subscribers_list;
}

} // namespace pubsub