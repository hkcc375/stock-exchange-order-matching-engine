#include "../include/stock_market.hpp"
#include <gtest/gtest.h>

using namespace Components;

// Tests getters and setters of Order class
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

// Tests getters and setters of Trade class
TEST(TradeTest, ConstructorAndGetters)
{
    auto ts = parseTimeString("11:00");
    Trade t(1, "AAPL", 101, 202, 150.0, 20, ts);

    EXPECT_EQ(t.getTradeID(), 1);
    EXPECT_EQ(t.getStockName(), "AAPL");
    EXPECT_EQ(t.getMatchedTradeBuyOrderID(), 101);
    EXPECT_EQ(t.getMatchedTradeSellOrderID(), 202);
    EXPECT_EQ(t.getTradePrice(), 150.0);
    EXPECT_EQ(t.getTradeQuantity(), 20);
    EXPECT_EQ(t.getTimestamp(), ts);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}