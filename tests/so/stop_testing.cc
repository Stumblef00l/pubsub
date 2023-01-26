#include <chrono>
#include <iostream>
#include <vector>

#include "gtest/gtest.h"

bool skip_testing = false;

// Class for test fixture
class MyTestFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    if (skip_testing) {
      GTEST_SKIP();
    }
  }
};

TEST_F(MyTestFixture, test1) {
  //
  EXPECT_EQ(1, 1);
}

TEST_F(MyTestFixture, footest) {
  EXPECT_EQ(1, 1);
  skip_testing = true;
}

TEST_F(MyTestFixture, test2) {
  //
  EXPECT_EQ(1, 1);
}