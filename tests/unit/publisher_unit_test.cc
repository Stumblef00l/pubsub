#include "pubsub/publisher.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/testing/mock_subscriber_family_manager.hpp"
#include "pubsub/testing/mock_subscriber_family.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"

namespace {

class BasicSynchronousPublisherUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            mock_family_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyManager>();
            mock_family_manager_ptr = mock_family_manager.get();
            mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
            publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(mock_family_manager));
        }
    
        std::unique_ptr<pubsub::BasicSynchronousPublisher> publisher;
        std::unique_ptr<pubsub::testing::MockSubscriberFamilyManager> mock_family_manager;
        pubsub::testing::MockSubscriberFamilyManager* mock_family_manager_ptr;
        std::unique_ptr<pubsub::testing::MockSubscriberFamily> mock_family;
};

TEST_F(BasicSynchronousPublisherUnitTest, SimpleGetSubscriberFamilyManager) {
    EXPECT_EQ(publisher->GetSubscriberFamilyManager(), mock_family_manager_ptr);
}

TEST_F(BasicSynchronousPublisherUnitTest, GetNullSubscriberFamilyManager) {
    publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(nullptr);

    EXPECT_THROW(publisher->GetSubscriberFamilyManager(), pubsub::NullSubscriberFamilyManagerException);
}

TEST_F(BasicSynchronousPublisherUnitTest, SimplePublish) {
    using ::testing::Return;

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

    EXPECT_NO_THROW(publisher->Publish(test_message));
}

} // namespace
