#include "pubsub/subscriber_family_manager.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/testing/mock_subscriber_family.hpp"

TEST(BasicSubscriberFamilyManagerUnitTest, SimpleCreateFamily) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto want_family_ptr = mock_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    const auto& family_id = want_family_ptr->GetID();

    family_manager->CreateFamily(std::move(mock_family));
    auto got_family_ptr = family_manager->GetFamily("family-1");

    EXPECT_EQ(want_family_ptr, got_family_ptr);
}

TEST(BasicSubscriberFamilyManagerUnitTest, CreateFamiliesWithSameName) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto mock_family_duplicate = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    
    auto mock_family_ptr = mock_family.get();
    auto mock_family_duplicate_ptr = mock_family_duplicate.get();

    // Same family names
    EXPECT_CALL((*mock_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    EXPECT_CALL((*mock_family_duplicate_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    family_manager->CreateFamily(std::move(mock_family));
    EXPECT_THROW(family_manager->CreateFamily(std::move(mock_family_duplicate)), pubsub::SubscriberFamilyAlreadyExistsException);
}

TEST(BasicSubscriberFamilyManagerUnitTest, SimpleDeleteFamily) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto want_family_ptr = mock_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    family_manager->CreateFamily(std::move(mock_family));
    family_manager->DeleteFamily("family-1");

    EXPECT_THROW(family_manager->GetFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);
}

TEST(BasicSubscriberFamilyManagerUnitTest, DeleteAlreadyDeletedFamily) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto want_family_ptr = mock_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    family_manager->CreateFamily(std::move(mock_family));
    family_manager->DeleteFamily("family-1");

    EXPECT_THROW(family_manager->DeleteFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);
}

TEST(BasicSubscriberFamilyManagerUnitTest, SimpleGetFamily) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto want_family_ptr = mock_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    const auto& family_id = want_family_ptr->GetID();

    family_manager->CreateFamily(std::move(mock_family));
    auto got_family_ptr = family_manager->GetFamily("family-1");

    EXPECT_EQ(want_family_ptr, got_family_ptr);
}

TEST(BasicSubscriberFamilyManagerUnitTest, GetNonExistentFamily) {
    using ::testing::Return;

    auto family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    
    EXPECT_THROW(family_manager->GetFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);

    auto mock_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    auto want_family_ptr = mock_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    const auto& family_id = want_family_ptr->GetID();

    family_manager->CreateFamily(std::move(mock_family));
    EXPECT_THROW(family_manager->GetFamily("family-2"), pubsub::SubscriberFamilyNotFoundException);
}
