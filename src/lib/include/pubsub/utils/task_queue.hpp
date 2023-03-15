#ifndef _PUBSUB_UTILS_TASK_QUEUE_
#define _PUBSUB_UTILS_TASK_QUEUE_

#include <exception>
#include <queue>
#include <shared_mutex>
#include <condition_variable>

#include "pubsub/structs.hpp"

namespace pubsub {
namespace utils {

template<class TaskType>
class IThreadSafeTaskQueue {
    public:
        // Waits (suspends thread) until queue is available for queueing again
        virtual void Enqueue(PubsubMessage message) = 0;

        // Waits (suspends thread) until queue becomes non-empty
        virtual TaskType Dequeue() = 0;
        virtual bool IsEmpty() const = 0;
        virtual bool IsFull() const = 0;

        virtual ~IThreadSafeMessageQueue() noexcept {}

    protected:
        IThreadSafeTaskQueue(const size_t capacity);

        const size_t capacity_;
};

template<class TaskType>
class OrderedThreadSafeTaskQueue: public IThreadSafeTaskQueue<TaskType> {
    public:
        OrderedThreadSafeTaskQueue(const size_t capacity);

        void Enqueue(PubsubMessage message) override;
        TaskType Dequeue() override;
        bool IsEmpty() const override;
        bool IsFull() const override;
    
    private:
        bool ThreadUnsafeIsEmpty() const;
        bool ThreadUnsafeIsFull() const;

        // We use a shared_mutex to allow multiple
        // concurrent reads
        mutable std::shared_mutex mtx;
        std::condition_variable_any queue_not_full_condition;
        std::condition_variable_any queue_not_empty_condition;

        std::queue<Task> backlog;
};

// Thrown when Queue is empty
class TaskQueueEmptyException: public std::exception {
    static constexpr std::string_view errorMessage = "Task queue has no queued tasks";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

// Thrown when Queue is full
class TaskQueueFullException: public std::exception {
    static constexpr std::string_view errorMessage = "Task queue is at full capacity";
    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
};

} // namespace utils
} // namespace pubsub

#endif