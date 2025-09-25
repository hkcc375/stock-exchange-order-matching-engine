#include "../include/stock_market.hpp"
#include <gtest/gtest.h>

using namespace Components;

// Tests getters and setters of Order class
TEST(OrderTest, ConstructorAndGettersMatchAsExpected)
{
    auto ts = parseTimeString("10:30");
    Order o("#1", "AAPL", OrderType::BUY, 100.5, 10, ts);

    EXPECT_EQ(o.getOrderID(), "#1");
    EXPECT_EQ(o.getStockName(), "AAPL");
    EXPECT_EQ(o.getPrice(), 100.5);
    EXPECT_EQ(o.getQuantity(), 10);
    EXPECT_EQ(o.getOrderType(), OrderType::BUY);
    EXPECT_EQ(o.getTimestamp(), ts);

    o.setQuantity(5);
    EXPECT_EQ(o.getQuantity(), 5);
}

// Tests getters and setters of Trade class
TEST(TradeTest, ConstructorAndGettersMatchAsExpected)
{
    auto ts = parseTimeString("11:00");
    Trade t(1, "AAPL", "#101", "#202", 150.0, 20, ts);

    EXPECT_EQ(t.getTradeID(), 1);
    EXPECT_EQ(t.getStockName(), "AAPL");
    EXPECT_EQ(t.getMatchedTradeBuyOrderID(), "#101");
    EXPECT_EQ(t.getMatchedTradeSellOrderID(), "#202");
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

// Fn : Checks if recordTrade and displayAllTrades works as expected;
TEST_F(TestOrderBook, RecordTradeMatchesExpected)
{
    auto timestamp = parseTimeString("10:00");

    testing::internal::CaptureStdout();
    tradeBook.recordTrade("AAPL", "#1", "#2", 150.0, 10, timestamp);
    tradeBook.displayAllTrades();
    std::string output = testing::internal::GetCapturedStdout();

    std::ostringstream expected;
    expected << "#2 " << "10 " << "150 " << "#1" << std::endl;

    EXPECT_EQ(output, expected.str());
}

// Fn : Checks if endOfDayCleanup works as expected;
TEST_F(TestOrderBook, EndOfDayCleanupMatchesExpected)
{

    auto marketTime = parseTimeString("13:00");
    orderBook.processOrderDetails("#1", "AAPL", OrderType::BUY, 150.0, 10, marketTime);
    orderBook.processOrderDetails("#2", "AAPL", OrderType::SELL, 160.0, 20, marketTime);

    testing::internal::CaptureStdout();
    orderBook.endOfDayCleanup();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("[EOD CANCEL]"), std::string::npos);
}

// Fn : Checks if processOrderDetails works as expected;
TEST_F(TestOrderBook, InvalidOrderDetailsMatchesExpected)
{
    auto ts = parseTimeString("12:00");

    testing::internal::CaptureStdout();
    orderBook.processOrderDetails("#1", "AAPL", OrderType::BUY, 0.0, 10, ts);
    std::string output = testing::internal::GetCapturedStdout();

    std::ostringstream expected;
    expected << "Invalid Order Details" << std::endl;

    EXPECT_EQ(output, expected.str());
}

// Fn : Checks if matchSell works as expected;
TEST_F(TestOrderBook, BuyMatchesSellMatchesExpected)
{
    auto marketTimeOne = parseTimeString("10:00");
    auto marketTimeTwo = parseTimeString("10:05");
    auto marketTimeThree = parseTimeString("10:10");
    auto marketTimeFour = parseTimeString("10:15");

    orderBook.processOrderDetails("#1", "AAPL", OrderType::BUY, 100.0, 10, marketTimeOne);
    orderBook.processOrderDetails("#2", "AAPL", OrderType::BUY, 102.0, 5, marketTimeTwo);
    orderBook.processOrderDetails("#3", "AAPL", OrderType::BUY, 101.0, 15, marketTimeThree);
    testing::internal::CaptureStdout();
    orderBook.processOrderDetails("#4", "AAPL", OrderType::SELL, 100.0, 20, marketTimeFour);
    tradeBook.displayAllTrades();
    std::string output = testing::internal::GetCapturedStdout();

    std::ostringstream expectedOne;
    expectedOne << "#4 " << "5 " << "102 " << "#2" << std::endl
                << "#4 " << "15 " << "101 " << "#3" << std::endl;

    EXPECT_EQ(output, expectedOne.str());
}

// Fn : Checks if matchBuy works as expected;
TEST_F(TestOrderBook, SellMatchesBuyMatchesExpected)
{
    auto marketTimeOne = parseTimeString("10:00");
    auto marketTimeTwo = parseTimeString("10:05");

    orderBook.processOrderDetails("#1", "AAPL", OrderType::BUY, 101.5, 5, marketTimeOne);
    testing::internal::CaptureStdout();
    orderBook.processOrderDetails("#2", "AAPL", OrderType::SELL, 100.0, 5, marketTimeTwo);
    tradeBook.displayAllTrades();
    std::string output = testing::internal::GetCapturedStdout();

    std::ostringstream expectedOne;
    expectedOne << "#2 " << "5 " << "101.5 " << "#1" << std::endl;

    EXPECT_EQ(output, expectedOne.str());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}