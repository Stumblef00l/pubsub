#include "pubsub/net/network_publisher_service.hpp"
#include "pubsub/net/network_subscriber_client.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"
#include "pubsub/subscriber_family.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"

namespace {

using namespace std::chrono_literals;

using pubsub::BasicThreadSafeSubscriberFamily;
using pubsub::BasicThreadSafeSubscriberFamilyManager;
using pubsub::BasicThreadSafeSubscriberFamilyRegistrationManager;
using pubsub::IThreadSafeSubscriberFamilyManager;
using pubsub::PubsubMessage;
using pubsub::SubscriberFamilyNotFoundException;
using pubsub::ThreadSafeRoundRobinSubscriberSelectionStrategy;
using pubsub::net::NetworkPublisherService;
using pubsub::net::NetworkSubscriberClient;

// A simple delegate ISubscriber that records every message it receives and
// lets test code block until an expected number have arrived.
class RecordingSubscriber: public pubsub::ISubscriber {
    public:
        RecordingSubscriber(): ISubscriber("") {}

        void
        Update(
            PubsubMessage message) override {
            std::lock_guard<std::mutex> lck(mtx_);
            received_.push_back(std::move(message));
            cv_.notify_all();
        }

        bool
        WaitForCount(
            size_t count,
            std::chrono::milliseconds timeout) {
            std::unique_lock<std::mutex> lck(mtx_);
            return cv_.wait_for(lck, timeout, [&] { return received_.size() >= count; });
        }

        std::vector<PubsubMessage>
        GetReceived() const {
            std::lock_guard<std::mutex> lck(mtx_);
            return received_;
        }

    private:
        mutable std::mutex mtx_;
        std::condition_variable cv_;
        std::vector<PubsubMessage> received_;
};

bool
WaitUntil(
    const std::function<bool()>& predicate,
    std::chrono::milliseconds timeout) {
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (predicate())
            return true;
        std::this_thread::sleep_for(5ms);
    }
    return predicate();
}

// Builds a NetworkPublisherService with a single thread-safe family
// registered under family_id, bound to an ephemeral localhost port. Returns
// the service along with a non-owning pointer to the family manager (now
// owned by the service) so tests can inspect registration state.
std::pair<std::unique_ptr<NetworkPublisherService>, IThreadSafeSubscriberFamilyManager*>
MakeServerWithFamily(
    const pubsub::PubsubSubscriberFamilyId& family_id,
    std::unique_ptr<pubsub::IThreadSafeSubscriberSelectionStrategy>& strategy_storage) {
    auto family_manager = std::make_unique<BasicThreadSafeSubscriberFamilyManager>();
    auto* family_manager_ptr = family_manager.get();

    strategy_storage = std::make_unique<ThreadSafeRoundRobinSubscriberSelectionStrategy>();
    auto registration_manager = std::make_unique<BasicThreadSafeSubscriberFamilyRegistrationManager>();

    auto family = std::make_unique<BasicThreadSafeSubscriberFamily>(
        family_id, std::move(registration_manager), strategy_storage.get());

    family_manager->CreateFamily(std::move(family));

    auto service = std::make_unique<NetworkPublisherService>(
        std::move(family_manager), "127.0.0.1", 0);

    return {std::move(service), family_manager_ptr};
}

TEST(NetworkPubsubIntegrationTest, RemoteClientReceivesPublishedMessage) {
    std::unique_ptr<pubsub::IThreadSafeSubscriberSelectionStrategy> strategy;
    auto [service, family_manager] = MakeServerWithFamily("family-1", strategy);

    RecordingSubscriber delegate;
    NetworkSubscriberClient client(
        "client-1", "family-1", &delegate, "127.0.0.1", service->GetBoundPort());

    ASSERT_TRUE(WaitUntil([&] { return client.IsConnected(); }, 2s));
    ASSERT_TRUE(WaitUntil([&] {
        return family_manager->GetFamily("family-1")->GetRegistrationManager()->GetSubscribers().size() == 1;
    }, 2s));

    service->Publish(PubsubMessage{"msg-1", "family-1", "hello-over-the-network"});

    ASSERT_TRUE(delegate.WaitForCount(1, 2s));
    EXPECT_EQ(delegate.GetReceived()[0], (PubsubMessage{"msg-1", "family-1", "hello-over-the-network"}));
}

TEST(NetworkPubsubIntegrationTest, MultipleRemoteClientsEachReceiveOneMessageViaRoundRobin) {
    std::unique_ptr<pubsub::IThreadSafeSubscriberSelectionStrategy> strategy;
    auto [service, family_manager] = MakeServerWithFamily("family-1", strategy);

    RecordingSubscriber delegate_a;
    RecordingSubscriber delegate_b;
    NetworkSubscriberClient client_a(
        "client-a", "family-1", &delegate_a, "127.0.0.1", service->GetBoundPort());
    NetworkSubscriberClient client_b(
        "client-b", "family-1", &delegate_b, "127.0.0.1", service->GetBoundPort());

    ASSERT_TRUE(WaitUntil([&] { return client_a.IsConnected() && client_b.IsConnected(); }, 2s));
    ASSERT_TRUE(WaitUntil([&] {
        return family_manager->GetFamily("family-1")->GetRegistrationManager()->GetSubscribers().size() == 2;
    }, 2s));

    service->Publish(PubsubMessage{"msg-1", "family-1", "first"});
    service->Publish(PubsubMessage{"msg-2", "family-1", "second"});

    ASSERT_TRUE(delegate_a.WaitForCount(1, 2s));
    ASSERT_TRUE(delegate_b.WaitForCount(1, 2s));
    EXPECT_EQ(delegate_a.GetReceived().size(), 1u);
    EXPECT_EQ(delegate_b.GetReceived().size(), 1u);
}

TEST(NetworkPubsubIntegrationTest, PublishingToUnknownFamilyThrows) {
    std::unique_ptr<pubsub::IThreadSafeSubscriberSelectionStrategy> strategy;
    auto [service, family_manager] = MakeServerWithFamily("family-1", strategy);

    EXPECT_THROW(
        service->Publish(PubsubMessage{"msg-1", "no-such-family", "payload"}),
        SubscriberFamilyNotFoundException);
}

TEST(NetworkPubsubIntegrationTest, ClientSubscribingToUnknownFamilyDoesNotCrashServiceOrBlockOtherClients) {
    std::unique_ptr<pubsub::IThreadSafeSubscriberSelectionStrategy> strategy;
    auto [service, family_manager] = MakeServerWithFamily("family-1", strategy);

    RecordingSubscriber bad_delegate;
    NetworkSubscriberClient bad_client(
        "bad-client", "no-such-family", &bad_delegate, "127.0.0.1", service->GetBoundPort());

    RecordingSubscriber good_delegate;
    NetworkSubscriberClient good_client(
        "good-client", "family-1", &good_delegate, "127.0.0.1", service->GetBoundPort());

    ASSERT_TRUE(WaitUntil([&] {
        return family_manager->GetFamily("family-1")->GetRegistrationManager()->GetSubscribers().size() == 1;
    }, 2s));

    service->Publish(PubsubMessage{"msg-1", "family-1", "payload"});

    ASSERT_TRUE(good_delegate.WaitForCount(1, 2s));
    EXPECT_TRUE(bad_delegate.GetReceived().empty());
}

} // namespace
