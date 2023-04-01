// Provides template implementation for pubsub/utils/task_queue.hpp. DO NOT IMPORT DIRECTLY
#ifndef _PUBSUB_UTILS_TASK_QUEUE_TPP_
#define _PUBSUB_UTILS_TASK_QUEUE_TPP_

#include "pubsub/utils/task_queue.hpp"

#include <queue>
#include <shared_mutex>
#include <utility>

#include "pubsub/structs.hpp"

namespace pubsub {
namespace utils {

template<class TaskType>
IThreadSafeTaskQueue<TaskType>::IThreadSafeTaskQueue(
    const size_t capacity)
    : capacity_(capacity),
      closed_(false) {}

template<class TaskType>
OrderedThreadSafeTaskQueue<TaskType>::OrderedThreadSafeTaskQueue(
    const size_t capacity,
    const bool drop_tasks_if_closed)
    : IThreadSafeTaskQueue<TaskType>(capacity),
      drop_tasks_if_closed_(drop_tasks_if_closed) {}

template<class TaskType>
OrderedThreadSafeTaskQueue<TaskType>::~OrderedThreadSafeTaskQueue() {
    Close();
}

template<class TaskType>
void
OrderedThreadSafeTaskQueue<TaskType>::Enqueue(
    TaskType message) {
    std::unique_lock lck { mtx_ };
    
    while (!(this->closed_) && (ThreadUnsafeGetSize() == this->capacity_))
        queue_not_full_condition_.wait(lck);
    
    if (this->closed_) {
        queue_not_empty_condition_.notify_all();
        queue_not_full_condition_.notify_all();
        throw TaskQueueClosedException();
    }

    backlog_.push(std::move(message));
    lck.unlock();

    queue_not_empty_condition_.notify_one();
}

template<class TaskType>
TaskType
OrderedThreadSafeTaskQueue<TaskType>::Dequeue() {
    std::unique_lock lck { mtx_ };

    while (!(this->closed_) && (ThreadUnsafeGetSize() == 0))
        queue_not_empty_condition_.wait(lck);

    if ((this->closed_) && (drop_tasks_if_closed_ || (ThreadUnsafeGetSize() == 0)))
        throw TaskQueueClosedException();
    
    if(this->closed_) {
        queue_not_full_condition_.notify_all();
        queue_not_empty_condition_.notify_all();
    }

    auto message = backlog_.front();
    backlog_.pop();
    lck.unlock();

    queue_not_full_condition_.notify_one();

    return message;
}

template<class TaskType>
bool
OrderedThreadSafeTaskQueue<TaskType>::IsEmpty() const {
    std::shared_lock lck { mtx_ };
    return (ThreadUnsafeGetSize() == 0); 
}

template<class TaskType>
bool
OrderedThreadSafeTaskQueue<TaskType>::IsFull() const {
    std::shared_lock lck { mtx_ };
    return (ThreadUnsafeGetSize() == this->capacity_); 
}

template<class TaskType>
size_t
OrderedThreadSafeTaskQueue<TaskType>::GetSize() const {
    std::shared_lock lck { mtx_ };
    return ThreadUnsafeGetSize();
}

template <class TaskType>
size_t
OrderedThreadSafeTaskQueue<TaskType>::ThreadUnsafeGetSize() const {
    return backlog_.size();
}

template <class TaskType>
bool
OrderedThreadSafeTaskQueue<TaskType>::Close() {
    std::unique_lock lck { mtx_ };
    bool retVal = !(this->closed_);
    this->closed_ = true;
    return retVal;
}

} // namespace utils
} // namespace pubsub

#endif