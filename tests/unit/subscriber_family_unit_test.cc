#include "pubsub/subscriber_family.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber.hpp"

TEST(BasicSubscriberFamilyUnitTest, GetID) {
    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        mock_selection_strategy.get());

    EXPECT_EQ((subscriber_family->GetID()).compare("test-family"), 0);
}

TEST(BasicSubscriberFamilyUnitTest, SimpleGetRegistrationManager) {
    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();

    auto registration_manager_ptr = mock_registration_manager.get();
    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        mock_selection_strategy.get());
    
    EXPECT_EQ(subscriber_family->GetRegistrationManager(), registration_manager_ptr);
}

TEST(BasicSubscriberFamilyUnitTest, GetNullRegistrationManager) {
    auto mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        nullptr,
        mock_selection_strategy.get());
    
    EXPECT_THROW(subscriber_family->GetRegistrationManager(), pubsub::NullSubscriberFamilyRegistrationManagerException);
}

TEST(BasicSubscriberFamilyUnitTest, SimpleGetSelectionStrategy) {
    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        mock_selection_strategy.get());
    
    EXPECT_EQ(subscriber_family->GetSelectionStrategy(), mock_selection_strategy.get());
}

TEST(BasicSubscriberFamilyUnitTest, GetNullSelectionStrategy) {
    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        nullptr);
    
    EXPECT_THROW(subscriber_family->GetSelectionStrategy(), pubsub::NullSubscriberFamilySelectionStrategyException);
}

TEST(BasicSubscriberFamilyUnitTest, SimpleSetSelectionStrategy) {
    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto mock_selection_strategy_1 = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    auto mock_selection_strategy_2 = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();

    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        mock_selection_strategy_1.get());

    EXPECT_NO_THROW(subscriber_family->SetSelectionStrategy(mock_selection_strategy_2.get()));
    EXPECT_EQ(subscriber_family->GetSelectionStrategy(), mock_selection_strategy_2.get());
    EXPECT_NE(subscriber_family->GetSelectionStrategy(), mock_selection_strategy_1.get());
}

TEST(BasicSubscriberFamilyUnitTest, SimplePublish) {
    using ::testing::Return;

    auto mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
    auto mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
    
    auto registration_manager_ptr = mock_registration_manager.get();
    auto selection_strategy_ptr = mock_selection_strategy.get();

    auto mock_subscriber = std::make_unique<pubsub::testing::MockSubscriber>();
    auto subscriber_ptr = mock_subscriber.get();
    auto ret_list = std::vector<pubsub::ISubscriber*>{ subscriber_ptr };

    auto subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        mock_selection_strategy.get());

    auto message = pubsub::PubsubMessage {
        "test-message",
        "test-family",
        "test-payload"
    };

    // Make sure Update is called only once.
    EXPECT_CALL((*subscriber_ptr), Update(message))
    .Times(1);

    EXPECT_CALL((*registration_manager_ptr), GetSubscribers())
    .WillRepeatedly(Return(ret_list));

    EXPECT_CALL((*selection_strategy_ptr), Select(ret_list))
    .WillRepeatedly(Return(ret_list));

    EXPECT_NO_THROW(subscriber_family->Publish(message));
}