#include "pubsub/publisher.hpp"

#include <exception>
#include <memory>
#include <thread>
#include <utility>

#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/utils/task_queue.hpp"


namespace pubsub {

IPublisher::IPublisher(
    IPublisher::ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : subscriber_family_manager_(std::move(subscriber_family_manager)) {}

IAsyncPublisher::IAsyncPublisher(
    ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : IPublisher(std::move(subscriber_family_manager)) {}

BasicSynchronousPublisher::BasicSynchronousPublisher(
    IPublisher::ISubscriberFamilyManagerUniquePtr subscriber_family_manager)
    : IPublisher(std::move(subscriber_family_manager)) {}

OrderedAsyncPublisher::OrderedAsyncPublisher(
    IThreadSafeSubscriberFamilyManagerUniquePtr subscriber_family_manager,
    IThreadSafeMessageQueueUniquePtr message_queue)
    : IAsyncPublisher(std::move(subscriber_family_manager)),
      message_queue_(std::move(message_queue)) {

    event_loop_thread_ = std::jthread(&OrderedAsyncPublisher::EventLoop, this);
}

OrderedAsyncPublisher::~OrderedAsyncPublisher() {
    Stop();
}

ISubscriberFamilyManager*
IPublisher::GetSubscriberFamilyManager() const {
    auto s = subscriber_family_manager_.get();
    if (s == nullptr || s == NULL)
        throw NullSubscriberFamilyManagerException();

    return s;
}

void
BasicSynchronousPublisher::Publish(
    PubsubMessage message) {
    auto s = subscriber_family_manager_.get();
    auto family = s->GetFamily(message.family_id);
    family->Publish(std::move(message));
}

void
OrderedAsyncPublisher::Publish(
    PubsubMessage message) {
    message_queue_->Enqueue(std::move(message));
}

void
OrderedAsyncPublisher::Stop() {
    message_queue_->Close();
}

void
OrderedAsyncPublisher::EventLoop() {
    while(true) {
        PubsubMessage message;
        try {
            message = message_queue_->Dequeue();
        } catch (const utils::TaskQueueClosedException& e) {
            return;
        }
        auto s = subscriber_family_manager_.get();
        auto family = s->GetFamily(message.family_id);
        family->Publish(std::move(message));
    }
}

} // namespace pubsub
