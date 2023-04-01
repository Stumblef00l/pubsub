#include "pubsub/subscriber_family_registration_manager.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "pubsub/subscriber.hpp"
#include "pubsub/testing/mock_subscriber.hpp"
#include "pubsub/utils/conversions.hpp"
#include "pubsub/utils/sort.hpp"

namespace {

class BasicSubscriberFamilyRegistrationManagerUnitTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            registration_manager = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>();
            mock_subscriber = std::make_unique<pubsub::testing::MockSubscriber>();
        }

        std::unique_ptr<pubsub::BasicSubscriberFamilyRegistrationManager> registration_manager;
        std::unique_ptr<pubsub::testing::MockSubscriber> mock_subscriber;
};

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, SimpleRegisterSubscriber) {
    using ::testing::Return;

    auto mock_subscriber_ptr = mock_subscriber.get();

    EXPECT_CALL((*mock_subscriber_ptr), GetID())
    .WillRepeatedly(Return("mock-subscriber"));

    EXPECT_NO_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr));
    EXPECT_EQ((mock_subscriber_ptr->GetID()).compare(registration_manager->GetSubscriber("mock-subscriber")->GetID()), 0);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, RegisterSubscriberTwice) {
    using ::testing::Return;

    auto mock_subscriber_ptr = mock_subscriber.get();

    EXPECT_CALL((*mock_subscriber_ptr), GetID())
    .WillRepeatedly(Return("mock-subscriber"));

    EXPECT_NO_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr));
    EXPECT_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr), pubsub::SubscriberAlreadyExistsException);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, SimpleUnregisterSubscriber) {
    using ::testing::Return;

    auto mock_subscriber_ptr = mock_subscriber.get();

    EXPECT_CALL((*mock_subscriber_ptr), GetID())
    .WillRepeatedly(Return("mock-subscriber"));

    EXPECT_NO_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr));
    EXPECT_NO_THROW(registration_manager->UnregisterSubscriber(mock_subscriber_ptr->GetID()));
    EXPECT_THROW(registration_manager->GetSubscriber(mock_subscriber_ptr->GetID()), pubsub::SubscriberNotFoundException);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, UnregisterSubscriberThatDoesNotExist) {
    using ::testing::Return;

    registration_manager = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>();
    EXPECT_THROW(registration_manager->UnregisterSubscriber("mock-subscriber"), pubsub::SubscriberNotFoundException);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, SimpleGetSubscriber) {
    using ::testing::Return;

    auto mock_subscriber_ptr = mock_subscriber.get();

    EXPECT_CALL((*mock_subscriber_ptr), GetID())
    .WillRepeatedly(Return("mock-subscriber"));

    EXPECT_NO_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr));
    EXPECT_EQ(registration_manager->GetSubscriber(mock_subscriber_ptr->GetID()), mock_subscriber_ptr);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, GetSubscriberThatDoesNotExist) {
    using ::testing::Return;

    registration_manager = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>(); 
    EXPECT_THROW(registration_manager->GetSubscriber("mock-subscriber"), pubsub::SubscriberNotFoundException);
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, SimpleGetSubscribers) {
    using ::testing::Return;

    auto subscribers_uptrs = std::vector<std::unique_ptr<pubsub::ISubscriber>>{};
    constexpr size_t SUBSCRIBER_COUNT = 5;

    for(auto idx = (size_t)0; idx < SUBSCRIBER_COUNT; idx++) {
        auto mock_subscriber = std::make_unique<pubsub::testing::MockSubscriber>();
        auto mock_subscriber_ptr = mock_subscriber.get();

        EXPECT_CALL((*mock_subscriber_ptr), GetID())
        .WillRepeatedly(Return("mock-subscriber" + std::to_string(idx)));

        subscribers_uptrs.push_back(std::move(mock_subscriber));
        EXPECT_NO_THROW(registration_manager->RegisterSubscriber(mock_subscriber_ptr));
    }

    auto want_ptr_list = pubsub::utils::ConvertUniquePtrListToRawPointerList(subscribers_uptrs);
    auto got_ptr_list = registration_manager->GetSubscribers();
    EXPECT_EQ(want_ptr_list.size(), got_ptr_list.size());

    std::sort(want_ptr_list.begin(), want_ptr_list.end(), pubsub::utils::LexicographicCompareSubscribers); 
    std::sort(got_ptr_list.begin(), got_ptr_list.end(), pubsub::utils::LexicographicCompareSubscribers); 

    for(auto idx = (size_t)0; idx < got_ptr_list.size(); idx++)
        EXPECT_EQ(want_ptr_list[idx]->GetID(), got_ptr_list[idx]->GetID());
}

TEST_F(BasicSubscriberFamilyRegistrationManagerUnitTest, GetNoSubscribers) {
    using ::testing::Return;

    registration_manager = std::make_unique<pubsub::BasicSubscriberFamilyRegistrationManager>();

    auto got_ptr_list = registration_manager->GetSubscribers();
    EXPECT_TRUE(got_ptr_list.empty());
}

} // namespace