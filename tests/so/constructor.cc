#include <gtest/gtest.h>

#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

template <class T>
class MyVector : public std::vector<T> {
 public:
  MyVector(std::initializer_list<T> il) : std::vector<T>(il) {}

  bool operator==(const std::vector<T>& other) const {
    // Need to static_cast so we don't call this function recursively
    if (this->size() < other.size()) {
      return (static_cast<const std::vector<T>&>(*this) ==
              std::vector<T>(other.begin(), other.begin() + this->size()));
    } else if (this->size() > other.size()) {
      return (std::vector<T>(this->begin(), this->begin() + other.size()) ==
              other);
    }
    return (static_cast<const std::vector<T>&>(*this) == other);
  }
};

TEST(SOME_TEST, TestMyVector) {
  MyVector<int> foo{1, 2, 3, 4};
  std::vector<int> bar{1, 2, 3, 4, 5};

  EXPECT_TRUE(foo == bar);

  EXPECT_TRUE(foo == (std::vector<int>{1, 2, 3, 4}));
  EXPECT_TRUE(foo == (std::vector<int>{1, 2, 3}));
  EXPECT_TRUE(foo == (std::vector<int>{1, 2, 3, 4, 5}));

  EXPECT_EQ(foo, bar);
  EXPECT_EQ(bar, foo);
}