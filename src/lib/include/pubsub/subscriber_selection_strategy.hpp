#ifndef _PUBSUB_SUBSCRIBER_SELECTION_STRATEGY_HPP_
#define _PUBSUB_SUBSCRIBER_SELECTION_STRATEGY_HPP_

#include <mutex>
#include <vector>

#include "subscriber.hpp"

namespace pubsub {

class ISubscriberSelectionStrategy {
    public:
        virtual std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) = 0;

        virtual ~ISubscriberSelectionStrategy() {}
};

class IThreadSafeSubscriberSelectionStrategy: public ISubscriberSelectionStrategy {
    public:
        virtual std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) = 0;
        
        virtual ~IThreadSafeSubscriberSelectionStrategy() {}
};

class RoundRobinSubscriberSelectionStrategy: public ISubscriberSelectionStrategy {
    public:
        RoundRobinSubscriberSelectionStrategy();

        std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) override;
    
    private:
        size_t current_selected_index_;
};

class ThreadSafeRoundRobinSubscriberSelectionStrategy: public IThreadSafeSubscriberSelectionStrategy {
    public:
        ThreadSafeRoundRobinSubscriberSelectionStrategy();

        std::vector<ISubscriber*> Select(const std::vector<ISubscriber*>& subscribers) override;

    private:
        size_t current_selected_index_;
        std::mutex index_mtx_;
};

} // namespace pubsub

#endif