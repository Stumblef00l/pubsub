// Provides template implementation for pubsub/utils/task_queue.hpp. DO NOT IMPORT DIRECTLY
#ifndef _PUBSUB_UTILS_TASK_QUEUE_TPP_
#define _PUBSUB_UTILS_TASK_QUEUE_TPP_

#include "pubsub/utils/task_queue.hpp"

#include <exception>
#include <queue>
#include <shared_mutex>
#include <condition_variable>

namespace pubsub {
namespace utils {

template<class TaskType>
IThreadSafeTaskQueue<TaskType>::IThreadSafeTaskQueue(const size_t capacity)
    : capacity_(capacity) {}

template<class TaskType>
OrderedThreadSafeTaskQueue<TaskType>::OrderedThreadSafeTaskQueue(const size_t capacity)
    : IThreadSafeTaskQueue<TaskType>(capacity) {}

template<class TaskType>
void OrderedThreadSafeTaskQueue<TaskType>::Enqueue(TaskType message) {
    std::unique_lock lck { mtx_ };
    
    while (ThreadUnsafeGetSize() == this->capacity_)
        queue_not_full_condition_.wait(lck);

    backlog_.push(std::move(message));
    lck.unlock();

    queue_not_empty_condition_.notify_one();
}

template<class TaskType>
TaskType OrderedThreadSafeTaskQueue<TaskType>::Dequeue() {
    std::unique_lock lck { mtx_ };

    while (ThreadUnsafeGetSize() == 0)
        queue_not_empty_condition_.wait(lck);

    auto message = backlog_.front();
    backlog_.pop();
    lck.unlock();

    queue_not_full_condition_.notify_one();

    return message;
}

template<class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::IsEmpty() const {
    std::shared_lock lck { mtx_ };
    return (ThreadUnsafeGetSize() == 0); 
}

template<class TaskType>
bool OrderedThreadSafeTaskQueue<TaskType>::IsFull() const {
    std::shared_lock lck { mtx_ };
    return (ThreadUnsafeGetSize() == this->capacity_); 
}

template<class TaskType>
size_t OrderedThreadSafeTaskQueue<TaskType>::GetSize() const {
    std::shared_lock lck { mtx_ };
    return ThreadUnsafeGetSize();
}

template <class TaskType>
size_t OrderedThreadSafeTaskQueue<TaskType>::ThreadUnsafeGetSize() const {
    return backlog_.size();
}

} // namespace utils
} // namespace pubsub

#endif