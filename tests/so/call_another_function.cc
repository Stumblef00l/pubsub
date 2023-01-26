// https://
// stackoverflow.com/questions/72355944/google-mock-invoke-function-with-parameters
#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

static bool helper(int a) {
  // do something here
  std::cout << "a is: " << a << std::endl;
  return true;
}

class ProxyMock {
 public:
  MOCK_METHOD1(functionA, bool(int a));
};

TEST(CallAnotherFunction, Test1) {
  ProxyMock mock;
  int a = 0;
  EXPECT_CALL(mock, functionA(5)).WillOnce([a]() { return helper(a); });
  mock.functionA(5);
}
