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

/* Fixtures in GoogleTest framework helps us avoid boilerplate construction of TradeBook class
   and OrderBook class in each of the below test functions;
   Similar to @BeforeAll in Mockito;
*/
class TestOrderBook : public ::testing::Test {
protected:
    TradeBook tradeBook;
    OrderBook orderBook;

    TestOrderBook()
        : orderBook(tradeBook)
    {
    }
};

TEST_F(TestOrderBook, RecordTradeMatchesExpected)
{
    EXPECT_EQ(tradeBook.getTrades().size(), 0);

    auto timestamp = parseTimeString("10:00");
    tradeBook.recordTrade("AAPL", 1, 2, 150.0, 10, timestamp);

    EXPECT_EQ(tradeBook.getTrades().size(), 1);
}

TEST_F(TestOrderBook, DisplayAllTradesMatchesExpected)
{
    auto timestamp = parseTimeString("10:00");
    tradeBook.recordTrade("AAPL", 1, 2, 150.0, 10, timestamp);

    // Captures output of displayAllTrades;
    testing::internal::CaptureStdout();
    tradeBook.displayAllTrades();
    std::string output = testing::internal::GetCapturedStdout();

    // Build the expected string;
    std::time_t tt = std::chrono::system_clock::to_time_t(timestamp);
    std::ostringstream expected;
    expected << "[TRADE]"
             << " id = 2"
             << " stockName = AAPL"
             << " buyOrderID = 1"
             << " sellOrderID = 2"
             << " Qty = 10"
             << " Price = 150"
             << " ts = " << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S") << std::endl;

    EXPECT_EQ(output, expected.str());
}

TEST_F(TestOrderBook, EndOfDayCleanupCancelsOrders)
{

    auto marketTime = parseTimeString("13:00");
    orderBook.processOrderDetails("AAPL", OrderType::BUY, 150.0, 10, marketTime);
    orderBook.processOrderDetails("AAPL", OrderType::SELL, 160.0, 20, marketTime);

    testing::internal::CaptureStdout();
    orderBook.endOfDayCleanup();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("[EOD CANCEL]"), std::string::npos);
    EXPECT_NE(output.find("All orders removed"), std::string::npos);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}