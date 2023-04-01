#include "pubsub/subscriber_selection_strategy.hpp"

#include <memory>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "pubsub/subscriber.hpp"
#include "pubsub/testing/mock_subscriber.hpp"
#include "pubsub/utils/conversions.hpp"

namespace {

class RoundRobinSelectionStrategyTest: public ::testing::Test {
    protected:
        void
        SetUp() override {
            for(auto idx = (size_t)0; idx < CANDIDATE_POOL_SIZE; idx++) {
                auto mock_candidate = std::make_unique<pubsub::testing::MockSubscriber>();
                mock_candidates.push_back(std::move(mock_candidate));
            }

            round_robin_strategy = std::make_unique<pubsub::RoundRobinSubscriberSelectionStrategy>();
        }

        static constexpr size_t CANDIDATE_POOL_SIZE = 5;
        std::vector<std::unique_ptr<pubsub::testing::MockSubscriber>> mock_candidates;
        std::unique_ptr<pubsub::RoundRobinSubscriberSelectionStrategy> round_robin_strategy;
};

TEST_F(RoundRobinSelectionStrategyTest, StaticListSelect) {
    using ::testing::Return;

    for(auto idx = (size_t)0; idx < mock_candidates.size(); idx++) {
        auto mock_candidate_ptr = mock_candidates[idx].get();

        EXPECT_CALL((*mock_candidate_ptr), GetID())
        .WillRepeatedly(Return("mock-subscriber-" + std::to_string(idx)));
    }

    // Round robin selection
    auto mock_candidates_raw_ptr = pubsub::utils::ConvertUniquePtrListToRawPointerList(mock_candidates);
    auto candidates_raw_ptr = std::vector<pubsub::ISubscriber*>{ mock_candidates_raw_ptr.begin(), mock_candidates_raw_ptr.end() };

    for(auto iteration = 0; iteration < 15; iteration++) { 
        auto got_list = round_robin_strategy->Select(candidates_raw_ptr);
        EXPECT_EQ(got_list.size(), 1);
        EXPECT_EQ((got_list[0]->GetID()).compare("mock-subscriber-" + std::to_string(iteration%CANDIDATE_POOL_SIZE)), 0);
    }
}

TEST_F(RoundRobinSelectionStrategyTest, EmptyListSelect) {
    using ::testing::Return;

    // Empty list
    auto empty_list = pubsub::utils::ConvertUniquePtrListToRawPointerList<pubsub::ISubscriber>({});
    auto got_list = round_robin_strategy->Select(empty_list);
    EXPECT_EQ(got_list.size(), 0);
}

} // namespace
