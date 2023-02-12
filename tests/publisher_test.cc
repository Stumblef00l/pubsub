#include <memory>

#include "gtest/gtest.h"
#include "pubsub/publisher.hpp"
#include "pubsub/subscriber_family_manager.hpp"

TEST(BasicSynchronousPublisherTest, GetSubscriberFamilyManager) {
    std::unique_ptr<pubsub::ISubscriberFamilyManager> sfMgr = std::make_unique<pubsub::BasicSubscriberFamilyManager>();
    auto sfMgrPtr = sfMgr.get();
    std::unique_ptr<pubsub::IPublisher> workingPublisher = std::make_unique<pubsub::BasicSynchronousPublisher>(std::move(sfMgr));
    
    EXPECT_EQ(workingPublisher->getSubscriberFamilyManager(), sfMgrPtr);
}

TEST(BasicSynchronousPublisherTest, GetNullSubscriberFamilyManager) {
    std::unique_ptr<pubsub::IPublisher> nullPublisher = std::make_unique<pubsub::BasicSynchronousPublisher>(nullptr);

    EXPECT_THROW(nullPublisher->getSubscriberFamilyManager(), pubsub::NullSubscriberFamilyManagerException);
}
