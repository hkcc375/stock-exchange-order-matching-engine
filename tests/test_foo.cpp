#include "../include/OrderMatchingEngine/foo.hpp"
#include <gtest/gtest.h>

using namespace OrderMatchingEngine;

TEST(FooTest, ValueCheck)
{
    Foo f(42);
    EXPECT_EQ(f.getValue(), 42);
}

TEST(FooTest, GreetingCheck)
{
    Foo f(7);
    EXPECT_EQ(f.greet(), "Hello from Foo, value = 7");
}
