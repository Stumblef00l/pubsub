#include "pubsub/utils/task_queue.hpp"

#include <future>
#include <memory>
#include <utility>

#include "gtest/gtest.h"

namespace {

class OrderedThreadSafeTaskQueueUnitTest: public ::testing::Test {
    protected:
        static constexpr size_t TEST_CAPACITY = 3;

        void SetUp() override {
            queue_ = std::make_unique<pubsub::utils::OrderedThreadSafeTaskQueue<int>>(TEST_CAPACITY);
        }

        std::unique_ptr<pubsub::utils::IThreadSafeTaskQueue<int>> queue_;
};

TEST_F(OrderedThreadSafeTaskQueueUnitTest, SimpleIsEmpty) {
    EXPECT_TRUE(queue_->IsEmpty());
}

TEST_F(OrderedThreadSafeTaskQueueUnitTest, SimpleIsFull) {
    for(auto i = (size_t)1; i <= TEST_CAPACITY; i++)
        queue_->Enqueue(i);

    EXPECT_TRUE(queue_->IsFull());
}

TEST_F(OrderedThreadSafeTaskQueueUnitTest, SyncGetSize) {
    while(!queue_->IsEmpty())
        queue_->Dequeue();
    
    EXPECT_EQ(queue_->GetSize(), 0);
    for(auto i = (size_t)1; i <= TEST_CAPACITY; i++) {
        queue_->Enqueue(i);
        EXPECT_EQ(queue_->GetSize(), i);
    }
    
    for(auto i = (size_t)1; i <= TEST_CAPACITY; i++) {
        EXPECT_EQ(queue_->Dequeue(), i);
        EXPECT_EQ(queue_->GetSize(), TEST_CAPACITY - i);
    }
}

TEST_F(OrderedThreadSafeTaskQueueUnitTest, AsyncTest) {
    // Ensure the queue is empty before the test
    while(!queue_->IsEmpty())
        queue_->Dequeue();
    EXPECT_TRUE(queue_->IsEmpty());

    auto enqueue_operation_vector = std::vector<std::future<void>>();
    auto dequeue_operation_vector = std::vector<std::future<int>>();

    // Queue more than capacity asynchronously
    for(auto i = (size_t)1; i <= TEST_CAPACITY + 1; i++)
        enqueue_operation_vector.push_back(
            std::async(
                std::launch::async,
                &pubsub::utils::IThreadSafeTaskQueue<int>::Enqueue,
                queue_.get(),
                i));
    
    // Dequeue twice
    for(auto i = (size_t)1; i <= 2; i++)
        dequeue_operation_vector.push_back(
            std::async(
                std::launch::async,
                &pubsub::utils::IThreadSafeTaskQueue<int>::Dequeue,
                queue_.get()));
    
    // Wait for all operations to finish
    for(auto &enqueue_operation: enqueue_operation_vector)
        enqueue_operation.wait();
    
    for(auto &dequeue_operation: dequeue_operation_vector)
        dequeue_operation.wait();

    // Final size should be one less than capacity
    EXPECT_EQ(queue_->GetSize(), TEST_CAPACITY - 1); 
}

} // namespace