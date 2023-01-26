
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Return;

//-----------------------------------------------------------------------------
// Real classes used in production
//-----------------------------------------------------------------------------
class OtherClass1 {
  //...
};

class OtherClass2 {
  //...
};

class ComplexVariable {
 public:
  ComplexVariable(OtherClass1 o1, OtherClass2 o2) {}
  //...
};

// Real foo used in production
struct foo {
  // Initializing alpha using other objects
  foo(OtherClass1 o1, OtherClass2 o2) : alpha(o1, o2) {}
  int SomeFunction() { return 0; }
  ComplexVariable alpha;
};

//-----------------------------------------------------------------------------
// Mocked classes used in test
//-----------------------------------------------------------------------------
// Mocked complex variable
struct MockedComplexVariable {
 public:
  MOCK_METHOD(int, get, (), ());
};

struct Mockfoo {
  // Mock Methods here
  // Not sure about variables??
 public:
  MOCK_METHOD(int, SomeFunction, (), ());
  MockedComplexVariable alpha;  // Doesn't need other objects!
};

template <typename foo>
class bar : public foo {
 public:
  // let's say foo has a member variable alpha
  // using foo::alpha in bar
  int UsingAlpha() { return foo::alpha.get() + foo::SomeFunction(); }
};



TEST(TestFoo, UsingAlpha) {
  bar<Mockfoo> b;
  EXPECT_CALL(b, SomeFunction()).WillOnce(Return(1)).WillOnce(Return(2));
  EXPECT_CALL(b.alpha, get()).WillOnce(Return(10)).WillOnce(Return(20));

  EXPECT_EQ(b.UsingAlpha(), 11);
  EXPECT_EQ(b.UsingAlpha(), 22);
}