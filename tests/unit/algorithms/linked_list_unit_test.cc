#include "algorithms/linked_list.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

namespace {

using IntList = algorithms::SinglyLinkedList<int>;

class SinglyLinkedListUnitTest: public ::testing::Test {
    protected:
        // Builds a bare node chain out of values, bypassing SinglyLinkedList,
        // so that the free reversal functions can be exercised directly
        template<class ValueType>
        static std::unique_ptr<algorithms::Node<ValueType>>
        BuildChain(
            const std::vector<ValueType>& values) {
            auto head = std::unique_ptr<algorithms::Node<ValueType>>{};
            for (auto value = values.rbegin(); value != values.rend(); value++) {
                auto node = std::make_unique<algorithms::Node<ValueType>>(*value);
                node->next = std::move(head);
                head = std::move(node);
            }
            return head;
        }

        template<class ValueType>
        static std::vector<ValueType>
        ChainToVector(
            const std::unique_ptr<algorithms::Node<ValueType>>& head) {
            auto values = std::vector<ValueType>{};
            for (auto* node = head.get(); node != nullptr; node = (node->next).get())
                values.push_back(node->value);
            return values;
        }

        static void
        ExpectReversesTo(
            const std::vector<int>& input,
            const std::vector<int>& expected) {
            auto iterative = algorithms::ReverseIteratively(BuildChain(input));
            EXPECT_EQ(ChainToVector(iterative), expected);

            auto recursive = algorithms::ReverseRecursively(BuildChain(input));
            EXPECT_EQ(ChainToVector(recursive), expected);
        }
};

TEST_F(SinglyLinkedListUnitTest, ReverseEmptyList) {
    ExpectReversesTo({}, {});
}

TEST_F(SinglyLinkedListUnitTest, ReverseSingleElementList) {
    ExpectReversesTo({7}, {7});
}

TEST_F(SinglyLinkedListUnitTest, ReverseTwoElementList) {
    ExpectReversesTo({1, 2}, {2, 1});
}

TEST_F(SinglyLinkedListUnitTest, ReverseManyElementList) {
    ExpectReversesTo({1, 2, 3, 4, 5}, {5, 4, 3, 2, 1});
}

TEST_F(SinglyLinkedListUnitTest, ReverseListWithDuplicates) {
    ExpectReversesTo({1, 2, 2, 3}, {3, 2, 2, 1});
}

TEST_F(SinglyLinkedListUnitTest, ReverseTwiceRestoresOriginalOrder) {
    const auto input = std::vector<int>{1, 2, 3, 4};

    auto list = IntList(input);
    list.Reverse();
    list.Reverse();

    EXPECT_EQ(list.ToVector(), input);
}

TEST_F(SinglyLinkedListUnitTest, ReverseKeepsSizeUnchanged) {
    auto list = IntList({1, 2, 3});

    list.Reverse();

    EXPECT_EQ(list.GetSize(), 3);
    EXPECT_FALSE(list.IsEmpty());
}

TEST_F(SinglyLinkedListUnitTest, ReverseUsingRecursionMatchesIterativeReverse) {
    const auto input = std::vector<int>{9, 8, 7, 6, 5};

    auto iterative_list = IntList(input);
    iterative_list.Reverse();

    auto recursive_list = IntList(input);
    recursive_list.ReverseUsingRecursion();

    EXPECT_EQ(recursive_list.ToVector(), iterative_list.ToVector());
}

TEST_F(SinglyLinkedListUnitTest, ReverseNonTrivialValueType) {
    auto list = algorithms::SinglyLinkedList<std::string>({"a", "b", "c"});

    list.Reverse();

    EXPECT_EQ(list.ToVector(), (std::vector<std::string>{"c", "b", "a"}));
}

TEST_F(SinglyLinkedListUnitTest, PushFrontBuildsListInReverseInsertionOrder) {
    auto list = IntList();
    EXPECT_TRUE(list.IsEmpty());

    for (auto value = 1; value <= 3; value++)
        list.PushFront(value);

    EXPECT_EQ(list.ToVector(), (std::vector<int>{3, 2, 1}));
}

TEST_F(SinglyLinkedListUnitTest, ReverseLongListIteratively) {
    static constexpr auto TEST_LENGTH = 100000;

    auto input = std::vector<int>{};
    input.reserve(TEST_LENGTH);
    for (auto value = 0; value < TEST_LENGTH; value++)
        input.push_back(value);

    auto list = IntList(input);
    list.Reverse();

    const auto reversed = list.ToVector();
    ASSERT_EQ(reversed.size(), input.size());
    EXPECT_EQ(reversed.front(), TEST_LENGTH - 1);
    EXPECT_EQ(reversed.back(), 0);
}

} // namespace
