// https://stackoverflow.com/questions/72382561/gmock-force-mocking-out-of-a-class-method-without-defining-and-referencing-a-mo/72395979#72395979
#include "gmock/gmock.h"
#include "gtest/gtest.h"

//-----------------------------------------------------------------------------
// Real classes used in production
//-----------------------------------------------------------------------------
class BottomClass {
 public:
  void do_something();
};

//-----------------------------------------------------------------------------
// Templatized classes used in test or production
//-----------------------------------------------------------------------------
template <class BType>
class MyHelperClass {
 public:
  BType bottom;

 public:
  void trigger_again() { bottom.do_something(); }
};

template <class BType, template <typename> class HType>
class MyConsumerClass {
 public:
  HType<BType> helper;

 public:
  void trigger() { helper.trigger_again(); }
};
//-----------------------------------------------------------------------------
// Mocked classes used in test
//-----------------------------------------------------------------------------
class MockedBottomClass {
 public:
  MOCK_METHOD(void, do_something, (), ());
};

TEST(BottomClassTest, Test1) {
  MyConsumerClass<MockedBottomClass, MyHelperClass> mock;
  EXPECT_CALL(mock.helper.bottom, do_something());

  mock.trigger();
}