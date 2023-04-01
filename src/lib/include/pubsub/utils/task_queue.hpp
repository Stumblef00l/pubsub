#ifndef _PUBSUB_UTILS_TASK_QUEUE_
#define _PUBSUB_UTILS_TASK_QUEUE_

#include <exception>
#include <queue>
#include <shared_mutex>
#include <condition_variable>

#include "pubsub/structs.hpp"

namespace pubsub {
namespace utils {

// Declarations

template<class TaskType>
class IThreadSafeTaskQueue {
    public:
        // Waits (suspends thread) until queue is available for queueing again
        // TODO(vkunal): Add support for non-blocking and blocking modes
        virtual void
        Enqueue(
            TaskType message) = 0;

        // Waits (suspends thread) until queue becomes non-empty
        // TODO(vkunal): Add support for non-blocking and blocking modes
        virtual TaskType
        Dequeue() = 0;
        
        virtual bool
        IsEmpty() const = 0;
        
        virtual bool
        IsFull() const = 0;
        
        virtual size_t
        GetSize() const = 0;
        
        virtual bool
        Close() = 0;

        virtual
        ~IThreadSafeTaskQueue() noexcept {}

    protected:
        IThreadSafeTaskQueue(
            const size_t capacity);

        const size_t capacity_;
        bool closed_;
};

template<class TaskType>
class OrderedThreadSafeTaskQueue: public IThreadSafeTaskQueue<TaskType> {
    public:
        OrderedThreadSafeTaskQueue(
            const size_t capacity,
            const bool drop_tasks_if_closed = true);
        
        ~OrderedThreadSafeTaskQueue();

        void
        Enqueue(
            TaskType message) override;
        
        TaskType
        Dequeue() override;
        
        bool
        IsEmpty() const override;
        
        bool
        IsFull() const override;
        
        size_t
        GetSize() const override;
        
        bool Close() override;
    
    private:
        size_t ThreadUnsafeGetSize() const;

        // We use a shared_mutex to allow multiple
        // concurrent reads
        mutable std::shared_mutex mtx_;
        std::condition_variable_any queue_not_full_condition_;
        std::condition_variable_any queue_not_empty_condition_;

        bool drop_tasks_if_closed_;
        std::queue<TaskType> backlog_;
};

// Thrown when Queue is empty
class TaskQueueEmptyException: public std::exception {    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
    
    private:
        static constexpr std::string_view errorMessage = "Task queue has no queued tasks";
};

// Thrown when Queue is full
class TaskQueueFullException: public std::exception {    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };
    
    private:
        static constexpr std::string_view errorMessage = "Task queue is at full capacity";
};

// Thrown when Task Queue is already closed
class TaskQueueClosedException: public std::exception {    
    public:
        // Displays the error message
        inline const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
            return errorMessage.data();
        };

    private:    
        static constexpr std::string_view errorMessage = "Task queue is closed and is not processing any more messages";
};

} // namespace utils
} // namespace pubsub

// Implementation
#include "pubsub/utils/task_queue.tpp"

#endif