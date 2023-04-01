#include "pubsub/subscriber_selection_strategy.hpp"

#include <mutex>
#include <vector>

#include "pubsub/subscriber.hpp"

namespace pubsub {

RoundRobinSubscriberSelectionStrategy::RoundRobinSubscriberSelectionStrategy()
    : current_selected_index_(0) {}

ThreadSafeRoundRobinSubscriberSelectionStrategy::ThreadSafeRoundRobinSubscriberSelectionStrategy()
    : current_selected_index_(0) {}

std::vector<ISubscriber*>
RoundRobinSubscriberSelectionStrategy::Select(
    const std::vector<ISubscriber*>& subscribers) {
    if(subscribers.empty())
        return {};
    
    if(current_selected_index_ >= subscribers.size())
        current_selected_index_ = 0;
    
    auto selected_subscribers_list = std::vector<ISubscriber*>{ subscribers[current_selected_index_++] };
    return selected_subscribers_list;
}

std::vector<ISubscriber*>
ThreadSafeRoundRobinSubscriberSelectionStrategy::Select(
    const std::vector<ISubscriber*>& subscribers) {
    if(subscribers.empty())
        return {};
    
    std::unique_lock lck { index_mtx_ };
    if(current_selected_index_ >= subscribers.size())
        current_selected_index_ = 0;
    
    auto selected_subscribers_list = std::vector<ISubscriber*>{ subscribers[current_selected_index_++] };
    lck.unlock();

    return selected_subscribers_list;
}

} // namespace pubsub