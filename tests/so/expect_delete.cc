// https://stackoverflow.com/questions/72542361/validate-if-delete-is-called-in-google-mock
#include "gmock/gmock.h"
#include "gtest/gtest.h"

//-----------------------------------------------------------------------------
// Defining the mock
//-----------------------------------------------------------------------------
// A dummy mock class whose do_delete function is called whenever delete[] is
// called.
struct MockDeletIndicator {
 public:
  MOCK_METHOD(void, do_delete, (void*), ());
};

// A global pointer that points to an object of type MockDeletIndicator.
MockDeletIndicator* global_delete_indicator = nullptr;
//-----------------------------------------------------------------------------
// Overloading the delete operator
//-----------------------------------------------------------------------------
// Globally overloading the delete operator. Not safe!
void operator delete[](void* p) throw() {
  if (global_delete_indicator != nullptr) {
    global_delete_indicator->do_delete(p);
  }
  free(p);
}
//-----------------------------------------------------------------------------
// Function under test
//-----------------------------------------------------------------------------
void freeMemory(char** test) {
  for (int i = 0; i < 5; i++) {
    delete[] test[i];
  }
  delete[] test;
}
//-----------------------------------------------------------------------------
// Tests
//-----------------------------------------------------------------------------
TEST(TestFoo, UsingAlpha) {
  // Creating a dynamic two-dimensional array.
  char** ary = new char*[5];
  for (int i = 0; i < 5; i++) {
    ary[i] = new char[5];
  }

  // Creating an object of MockDeletIndicator an store its address in
  // global_delete_indicator
  MockDeletIndicator mock_delete_indicator;
  global_delete_indicator = &mock_delete_indicator;

  // Validating if delete[] is called.
  for (int i = 0; i < 5; i++) {
    EXPECT_CALL(mock_delete_indicator, do_delete(ary[i]));
  }
  EXPECT_CALL(mock_delete_indicator, do_delete(ary));

  // Call function under test.
  freeMemory(ary);
}