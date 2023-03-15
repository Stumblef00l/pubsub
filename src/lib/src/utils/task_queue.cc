#include "pubsub/utils/task_queue.hpp"

namespace pubsub {
namespace utils {

template<class TaskType>
IThreadSafeTaskQueue<TaskType>::IThreadSafeTaskQueue(const size_t capacity)
    : capacity_(capacity) {}

template<class TaskType>
OrderedThreadSafeTaskQueue<TaskType>::OrderedThreadSafeTaskQueue(const size_t capacity)
    : IThreadSafeTaskQueue(capacity) {}

template<class TaskType>
void OrderedThreadSafeTaskQueue<TaskType>::Enqueue(PubsubMessage message) {
    std::unique_lock lck { mtx };
    
    while (ThreadUnsafeIsFull())
        queue_not_full_condition.wait(lck);

    backlog.push(std::move(message));
}

template<class TaskType>
TaskType OrderedThreadSafeTaskQueue<TaskType>::Dequeue() {
    std::unique_lock lck { mtx };

    while (ThreadUnsafeIsEmpty())
        queue_not_empty_condition.wait(lck);

    auto message = backlog.front();
    backlog.pop();
    lck.unlock();

    return message;
}

template<class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::IsEmpty() const {
    std::shared_lock lck { mtx };
    return ThreadUnsafeIsEmpty(); 
}

template<class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::IsFull() const {
    std::shared_lock lck { mtx };
    return ThreadUnsafeIsFull(); 
}

template <class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::ThreadUnsafeIsEmpty() const {
    return backlog.empty();
}

template <class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::ThreadUnsafeIsFull() const {
    return (backlog.size() == capacity_);
}

} // namespace utils
} // namespace pubsub