#include "../include/stock_market.hpp"
#include <gtest/gtest.h>

using namespace Components;

TEST(OrderTest, ConstructorAndGetters)
{
    auto ts = parseTimeString("10:30");
    Order o(1, "AAPL", OrderType::BUY, 100.5, 10, ts);

    EXPECT_EQ(o.getOrderID(), 1);
    EXPECT_EQ(o.getStockName(), "AAPL");
    EXPECT_EQ(o.getPrice(), 100.5);
    EXPECT_EQ(o.getQuantity(), 10);
    EXPECT_EQ(o.getOrderType(), OrderType::BUY);
    EXPECT_EQ(o.getTimestamp(), ts);

    o.setQuantity(5);
    EXPECT_EQ(o.getQuantity(), 5);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv); // Initialize gtest
    return RUN_ALL_TESTS(); // Run all TEST() cases
}