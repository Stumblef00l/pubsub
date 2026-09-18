#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

#include "pubsub/net/network_publisher_service.hpp"
#include "pubsub/net/network_subscriber_client.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"
#include "pubsub/subscriber_family.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"

// The application-level subscriber logic. This is unaware that messages are
// arriving over the network -- NetworkSubscriberClient hands it plain
// PubsubMessages exactly as a local, in-memory subscriber family would.
class PrintingSubscriber: public pubsub::ISubscriber {
    public:
        PrintingSubscriber(pubsub::PubsubSubscriberId id): ISubscriber(std::move(id)) {}

        void Update(pubsub::PubsubMessage message) override {
            std::cout << "I'm subscriber " << id_ << " and I've received a message over the network: "
                       << "{ id: " << message.id
                       << ", family_id: " << message.family_id
                       << ", payload: " << message.payload << " }" << std::endl;
        }
};

int main() {
    // --- Server side: a NetworkPublisherService is an IPublisher that also
    // listens on a TCP port for remote subscriber clients. ---

    auto family_manager = std::make_unique<pubsub::BasicThreadSafeSubscriberFamilyManager>();

    auto strategy = std::make_unique<pubsub::ThreadSafeRoundRobinSubscriberSelectionStrategy>();
    auto registration_manager = std::make_unique<pubsub::BasicThreadSafeSubscriberFamilyRegistrationManager>();
    auto family = std::make_unique<pubsub::BasicThreadSafeSubscriberFamily>(
        "family-1", std::move(registration_manager), strategy.get());
    family_manager->CreateFamily(std::move(family));

    pubsub::net::NetworkPublisherService service(
        std::move(family_manager), "127.0.0.1", /*bind_port=*/0);

    std::cout << "Publisher service listening on 127.0.0.1:" << service.GetBoundPort() << std::endl;

    // --- Client side: a NetworkSubscriberClient connects out to the
    // service and forwards received messages to a local delegate. ---

    PrintingSubscriber delegate("subscriber-1");
    pubsub::net::NetworkSubscriberClient client(
        "subscriber-1", "family-1", &delegate, "127.0.0.1", service.GetBoundPort());

    while (!client.IsConnected())
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // Publish a few messages; the client above receives them asynchronously
    // over the network and prints them via PrintingSubscriber::Update.
    service.Publish(pubsub::PubsubMessage{"first-message", "family-1", "hello over the wire"});
    service.Publish(pubsub::PubsubMessage{"second-message", "family-1", "another message"});

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    return 0;
}
