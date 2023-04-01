#include "pubsub/subscriber_family.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/structs.hpp"
#include "pubsub/subscriber.hpp"
#include "pubsub/testing/mock_subscriber_family_registration_manager.hpp"
#include "pubsub/testing/mock_subscriber_selection_strategy.hpp"
#include "pubsub/testing/mock_subscriber.hpp"

namespace {

class BasicSubscriberFamilyUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            mock_registration_manager = std::make_unique<pubsub::testing::MockSubscriberFamilyRegistrationManager>();
            mock_registration_manager_ptr = mock_registration_manager.get();
            mock_selection_strategy = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();
            mock_subscriber = std::make_unique<pubsub::testing::MockSubscriber>();
            subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
                "test-family",
                std::move(mock_registration_manager), 
                mock_selection_strategy.get());
        }

        std::unique_ptr<pubsub::testing::MockSubscriberFamilyRegistrationManager> mock_registration_manager;
        pubsub::testing::MockSubscriberFamilyRegistrationManager* mock_registration_manager_ptr;
        std::unique_ptr<pubsub::testing::MockSubscriberSelectionStrategy> mock_selection_strategy;
        std::unique_ptr<pubsub::testing::MockSubscriber> mock_subscriber;
        std::unique_ptr<pubsub::BasicSubscriberFamily> subscriber_family;
};

TEST_F(BasicSubscriberFamilyUnitTest, GetID) {
    EXPECT_EQ((subscriber_family->GetID()).compare("test-family"), 0);
}

TEST_F(BasicSubscriberFamilyUnitTest, SimpleGetRegistrationManager) {
    EXPECT_EQ(subscriber_family->GetRegistrationManager(), mock_registration_manager_ptr);
}

TEST_F(BasicSubscriberFamilyUnitTest, GetNullRegistrationManager) {
    subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        nullptr,
        mock_selection_strategy.get());
    
    EXPECT_THROW(subscriber_family->GetRegistrationManager(), pubsub::NullSubscriberFamilyRegistrationManagerException);
}

TEST_F(BasicSubscriberFamilyUnitTest, SimpleGetSelectionStrategy) {
    EXPECT_EQ(subscriber_family->GetSelectionStrategy(), mock_selection_strategy.get());
}

TEST_F(BasicSubscriberFamilyUnitTest, GetNullSelectionStrategy) {
    subscriber_family = std::make_unique<pubsub::BasicSubscriberFamily>(
        "test-family",
        std::move(mock_registration_manager),
        nullptr);
    
    EXPECT_THROW(subscriber_family->GetSelectionStrategy(), pubsub::NullSubscriberFamilySelectionStrategyException);
}

TEST_F(BasicSubscriberFamilyUnitTest, SimpleSetSelectionStrategy) {
    auto mock_selection_strategy_2 = std::make_unique<pubsub::testing::MockSubscriberSelectionStrategy>();

    EXPECT_NO_THROW(subscriber_family->SetSelectionStrategy(mock_selection_strategy_2.get()));
    EXPECT_EQ(subscriber_family->GetSelectionStrategy(), mock_selection_strategy_2.get());
    EXPECT_NE(subscriber_family->GetSelectionStrategy(), mock_selection_strategy.get());
}

TEST_F(BasicSubscriberFamilyUnitTest, SimplePublish) {
    using ::testing::Return;
    auto mock_selection_strategy_ptr = mock_selection_strategy.get();

    auto mock_subscriber_ptr = mock_subscriber.get();
    auto ret_list = std::vector<pubsub::ISubscriber*>{ mock_subscriber_ptr };

    auto message = pubsub::PubsubMessage {
        "test-message",
        "test-family",
        "test-payload"
    };

    // Make sure Update is called only once.
    EXPECT_CALL((*mock_subscriber_ptr), Update(message))
    .Times(1);

    EXPECT_CALL((*mock_registration_manager_ptr), GetSubscribers())
    .WillRepeatedly(Return(ret_list));

    EXPECT_CALL((*mock_selection_strategy_ptr), Select(ret_list))
    .WillRepeatedly(Return(ret_list));

    EXPECT_NO_THROW(subscriber_family->Publish(message));
}

} // namespace