#include "pubsub/subscriber_selection_strategy.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "pubsub/subscriber.hpp"
#include "pubsub/testing/mock_subscriber.hpp"
#include "pubsub/utils/conversions.hpp"

TEST(RoundRobinSelectionStrategyTest, StaticListSelect) {
    using ::testing::Return;

    constexpr size_t CANDIDATE_POOL_SIZE = 5;
    auto selection_strategy = std::make_unique<pubsub::RoundRobinSubscriberSelectionStrategy>();

    auto candidates = std::vector<std::unique_ptr<pubsub::ISubscriber>>{};
    for(auto idx = (size_t)0; idx < CANDIDATE_POOL_SIZE; idx++) {
        auto candidate = std::make_unique<pubsub::testing::MockSubscriber>();
        auto candidate_ptr = candidate.get();

        EXPECT_CALL((*candidate_ptr), GetID())
        .WillRepeatedly(Return("subscriber-" + std::to_string(idx)));

        candidates.push_back(std::move(candidate));
    }

    // Round robin selection
    auto candidates_raw_ptr = pubsub::utils::ConvertUniquePtrListToRawPointerList(candidates);
    
    for(auto iteration = 0; iteration < 15; iteration++) { 
        auto got_list = selection_strategy->Select(candidates_raw_ptr);
        EXPECT_EQ(got_list.size(), 1);
        EXPECT_EQ((got_list[0]->GetID()).compare("subscriber-" + std::to_string(iteration%CANDIDATE_POOL_SIZE)), 0);
    }
}

TEST(RoundRobinSelectionStrategyTest, EmptyListSelect) {
    using ::testing::Return;

    auto selection_strategy = std::make_unique<pubsub::RoundRobinSubscriberSelectionStrategy>();

    // Empty list
    auto empty_list = pubsub::utils::ConvertUniquePtrListToRawPointerList<pubsub::ISubscriber>({});
    auto got_list = selection_strategy->Select(empty_list);
    EXPECT_EQ(got_list.size(), 0);
}