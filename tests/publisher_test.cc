#include <memory>

#include "gtest/gtest.h"
#include "pubsub/publisher.hpp"

TEST(BasicSynchronousPublisherTest, GetSubscriberFamilyManager) {
    std::unique_ptr<pubsub::IPublisher> publisher = std::make_unique<pubsub::BasicSynchronousPublisher>(nullptr);

    EXPECT_THROW(publisher->getSubscriberFamilyManager(), pubsub::NullSubscriberFamilyManagerException);
}
