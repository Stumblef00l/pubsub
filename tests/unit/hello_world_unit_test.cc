#include "hello_world/hello_world.hpp"

#include <string>

#include "gtest/gtest.h"

namespace {

TEST(HelloWorldUnitTest, HelloWorldGreetsTheWorld) {
    EXPECT_EQ(hello_world::HelloWorld(), "Hello, World!");
}

TEST(HelloWorldUnitTest, GreetUsesTheGivenName) {
    EXPECT_EQ(hello_world::Greet("subscriber-1"), "Hello, subscriber-1!");
}

TEST(HelloWorldUnitTest, GreetFallsBackToTheWorldOnEmptyName) {
    EXPECT_EQ(hello_world::Greet(""), hello_world::HelloWorld());
}

} // namespace
