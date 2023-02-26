#include "pubsub/subscriber_family_manager.hpp"

#include <memory>
#include <utility>

#include "gtest/gtest.h"
#include "pubsub/testing/mock_subscriber_family.hpp"

namespace {

class BasicSubscriberFamilyManagerUnitTest: public ::testing::Test {
    protected:
        void SetUp() override {
            subscriber_family = std::make_unique<pubsub::testing::MockSubscriberFamily>();
            family_manager = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
        }

        std::unique_ptr<pubsub::testing::MockSubscriberFamily> subscriber_family;
        std::unique_ptr<pubsub::BasicSubscriberFamilyManager> family_manager;
};

TEST_F(BasicSubscriberFamilyManagerUnitTest, SimpleCreateFamily) {
    using ::testing::Return;

    auto want_family_ptr = subscriber_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    EXPECT_NO_THROW(family_manager->CreateFamily(std::move(subscriber_family)));
    auto got_family_ptr = family_manager->GetFamily("family-1");

    EXPECT_EQ(want_family_ptr, got_family_ptr);
}

TEST_F(BasicSubscriberFamilyManagerUnitTest, CreateFamiliesWithSameName) {
    using ::testing::Return;

    auto family_duplicate = std::make_unique<pubsub::testing::MockSubscriberFamily>();
    
    auto family_ptr = subscriber_family.get();
    auto family_duplicate_ptr = family_duplicate.get();

    // Same family names
    EXPECT_CALL((*family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    EXPECT_CALL((*family_duplicate_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    family_manager->CreateFamily(std::move(subscriber_family));
    EXPECT_THROW(family_manager->CreateFamily(std::move(family_duplicate)), pubsub::SubscriberFamilyAlreadyExistsException);
}

TEST_F(BasicSubscriberFamilyManagerUnitTest, SimpleDeleteFamily) {
    using ::testing::Return;

    auto want_family_ptr = subscriber_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    family_manager->CreateFamily(std::move(subscriber_family));
    family_manager->DeleteFamily("family-1");

    EXPECT_THROW(family_manager->GetFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);
}

TEST_F(BasicSubscriberFamilyManagerUnitTest, DeleteAlreadyDeletedFamily) {
    using ::testing::Return;

    auto want_family_ptr = subscriber_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    EXPECT_NO_THROW(family_manager->CreateFamily(std::move(subscriber_family)));
    EXPECT_NO_THROW(family_manager->DeleteFamily("family-1"));

    EXPECT_THROW(family_manager->DeleteFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);
}

TEST_F(BasicSubscriberFamilyManagerUnitTest, SimpleGetFamily) {
    using ::testing::Return;

    auto want_family_ptr = subscriber_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    const auto& family_id = want_family_ptr->GetID();

    EXPECT_NO_THROW(family_manager->CreateFamily(std::move(subscriber_family)));
    EXPECT_EQ(want_family_ptr, family_manager->GetFamily("family-1"));
}

TEST_F(BasicSubscriberFamilyManagerUnitTest, GetNonExistentFamily) {
    using ::testing::Return;
    
    EXPECT_THROW(family_manager->GetFamily("family-1"), pubsub::SubscriberFamilyNotFoundException);

    auto want_family_ptr = subscriber_family.get();
    
    EXPECT_CALL((*want_family_ptr), GetID())
    .WillRepeatedly(Return("family-1"));

    EXPECT_NO_THROW(family_manager->CreateFamily(std::move(subscriber_family)));
    EXPECT_THROW(family_manager->GetFamily("family-2"), pubsub::SubscriberFamilyNotFoundException);
}

} // namesapce
