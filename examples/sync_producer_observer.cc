#include <iostream>
#include <memory>
#include <utility>

#include "pubsub/publisher.hpp"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/subscriber_family_registration_manager.hpp"
#include "pubsub/subscriber_family.hpp"
#include "pubsub/subscriber_selection_strategy.hpp"
#include "pubsub/subscriber.hpp"

// Implement a subscriber along with the update function
class PrintingSubscriber: public pubsub::ISubscriber {
    public:
        PrintingSubscriber(pubsub::PubsubSubscriberId id): ISubscriber(std::move(id)) {}

        // This contains your main subscriber side business logic
        void Update(pubsub::PubsubMessage message) override {
            std::cout << "I'm subscriber " << id_ << " and I've received a messaage: ";
            DisplayPubsubMessage(std::move(message));
        }

    private:
        void DisplayPubsubMessage(const pubsub::PubsubMessage& message) const {
            std::cout << "{" << std::endl;
            std::cout << "  id: " << message.id << std::endl;
            std::cout << "  family_id: " << message.family_id << std::endl;
            std::cout << "  payload: " << message.payload << std::endl;
            std::cout << "}" << std::endl;
        }
};

int main() {
    // --- INIT PHASE ---
    
    // **Step 1: Create a Publisher instance**

    // 1. Create Subscriber Family Manager
    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();

    // 2. Create the Publisher instance
    auto publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(family_manager));

    // **Step 2: Create Subscriber Families**

    // 1. Initializing strategy (one per family in this case in order to avoid sharing state
    // across families)
    auto family_1_strategy = std::make_unique<pubsub::RoundRobinSubscriberSelectionStrategy>();
    auto family_2_strategy = std::make_unique<pubsub::RoundRobinSubscriberSelectionStrategy>();

    // 2. Initializing registration managers
    auto family_1_rm = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>();
    auto family_2_rm = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>();
    
    // 3. Initializing subscriber families
    auto family_1 = std::make_unique<pubsub::BasicSubscriberFamily>(
        "family-1",
        std::move(family_1_rm),
        family_1_strategy.get());
    auto family_2 = std::make_unique<pubsub::BasicSubscriberFamily>(
        "family-2",
        std::move(family_2_rm),
        family_2_strategy.get());

    // 4. Adding subscriber families to the publisher
    publisher
    ->GetSubscriberFamilyManager()
    ->CreateFamily(std::move(family_1));
    
    publisher
    ->GetSubscriberFamilyManager()
    ->CreateFamily(std::move(family_2));

    // --- USAGE PHASE ---

    // Create Subscribers
    auto subscriber_1 = std::make_unique<PrintingSubscriber>("subscriber-1");
    auto subscriber_2 = std::make_unique<PrintingSubscriber>("subscriber-2");
    auto subscriber_3 = std::make_unique<PrintingSubscriber>("subscriber-3");
    
    // Register Subscribers (2 to family-1 and 1 to family-2)
    publisher
    ->GetSubscriberFamilyManager()
    ->GetFamily("family-1")
    ->GetRegistrationManager()
    ->RegisterSubscriber(subscriber_1.get());
    
    publisher
    ->GetSubscriberFamilyManager()
    ->GetFamily("family-1")
    ->GetRegistrationManager()
    ->RegisterSubscriber(subscriber_2.get());
    
    publisher
    ->GetSubscriberFamilyManager()
    ->GetFamily("family-2")
    ->GetRegistrationManager()
    ->RegisterSubscriber(subscriber_3.get());

    // Publish first message
    publisher->Publish(pubsub::PubsubMessage {
        "first-message",
        "family-1",
        "first_message"
    });

    // Publish second message
    publisher->Publish(pubsub::PubsubMessage {
        "second-message",
        "family-1",
        "second_message"
    });

    // Publish third message (family-2)
    publisher->Publish(pubsub::PubsubMessage {
        "second-message",
        "family-2",
        "second_message"
    });

    // Unregister subscriber-1
    publisher
    ->GetSubscriberFamilyManager()
    ->GetFamily("family-1")
    ->GetRegistrationManager()
    ->UnregisterSubscriber(subscriber_1->GetID());

    // Publish fourth message
    publisher->Publish(pubsub::PubsubMessage {
        "fourth-message",
        "family-1",
        "fourth_message"
    });

    // Publish fifth message
    publisher->Publish(pubsub::PubsubMessage {
        "fifth-message",
        "family-1",
        "fifth_message"
    });

    // Unregister subscriber-3
    publisher
    ->GetSubscriberFamilyManager()
    ->GetFamily("family-2")
    ->GetRegistrationManager()
    ->UnregisterSubscriber(subscriber_3->GetID());

    // Publish sixth message (should not display as there is no subscriber)
    publisher->Publish(pubsub::PubsubMessage {
        "sixth-message",
        "family-2",
        "sixth_message"
    });
    
    return 0;
}