#include <memory>

#include "gtest/gtest.h"
#include "pubsub/publisher.hpp"
#include "pubsub/subscriber_family_manager.hpp"
#include "pubsub/testing/mock_subscriber_family_manager.hpp"
#include "pubsub/testing/mock_subscriber_family.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"


TEST(BasicSynchronousPublisherUnitTest, GetSubscriberFamilyManager) {
    auto sfMgr = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto sfMgrPtr = sfMgr.get();
    auto workingPublisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(sfMgr));
    
    EXPECT_EQ(workingPublisher->GetSubscriberFamilyManager(), sfMgrPtr);
}

TEST(BasicSynchronousPublisherUnitTest, GetNullSubscriberFamilyManager) {
    auto nullPublisher = std::make_unique<pubsub::BasicSynchronousPublisher>(nullptr);

    EXPECT_THROW(nullPublisher->GetSubscriberFamilyManager(), pubsub::NullSubscriberFamilyManagerException);
}

TEST(BasicSynchronousPublisherUnitTest, PublishWithoutBehavior) {
    using ::testing::Return;

    auto mockSFMgr = std::make_unique<pubsub::testing::MockSubscriberFamilyManager>();
    auto mockSFMgrPtr = mockSFMgr.get();
    auto publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(mockSFMgr));

    auto mock_selection = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();

    auto mockSF = std::make_unique<pubsub::testing::MockSubscriberFamily>(
        "test-family-1",
        std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>(),
        mock_selection.get()
    );

    auto test_message = pubsub::PubsubMessage {
        "test-1",
        "test-family-1",
        "test-payload"
    };

    EXPECT_CALL((*mockSFMgrPtr), GetFamily(test_message.family_id))
    .Times(1)
    .WillOnce(Return(mockSF.get()));

    EXPECT_CALL((*mockSF), Publish(test_message))
    .Times(1);

    publisher->Publish(test_message);
}
