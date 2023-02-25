#include "pubsub/publisher.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/testing/mock_subscriber_family_manager.hpp"
#include "pubsub/testing/mock_subscriber_family.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"


TEST(BasicSynchronousPublisherUnitTest, GetSubscriberFamilyManager) {
    auto family_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyManager>();
    auto family_manager_ptr = family_manager.get();
    auto publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(family_manager));
    
    EXPECT_EQ(publisher->GetSubscriberFamilyManager(), family_manager_ptr);
}

TEST(BasicSynchronousPublisherUnitTest, GetNullSubscriberFamilyManager) {
    auto publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(nullptr);

    EXPECT_THROW(publisher->GetSubscriberFamilyManager(), pubsub::NullSubscriberFamilyManagerException);
}

TEST(BasicSynchronousPublisherUnitTest, SimplePublish) {
    using ::testing::Return;

    auto mock_family_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyManager>();
    auto mock_family_manager_ptr = mock_family_manager.get();
    auto publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(mock_family_manager));

    auto mock_selection = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();

    auto test_message = pubsub::PubsubMessage {
        "test-1",
        "test-family-1",
        "test-payload"
    };

    EXPECT_CALL((*mock_family_manager_ptr), GetFamily(test_message.family_id))
    .Times(1)
    .WillOnce(Return(mock_family.get()));

    EXPECT_CALL((*mock_family), Publish(test_message))
    .Times(1);

    publisher->Publish(test_message);
}
