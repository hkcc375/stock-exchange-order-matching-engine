#include "../include/stock_market.hpp"
#include <iostream>
using namespace Components;

int main()
{
    // Create TradeBook
    TradeBook tradeBook;

    // Inject TradeBook into OrderBook
    OrderBook orderBook(tradeBook);

    // Add some BUY orders
    orderBook.processOrderDetails(101, OrderType::BUY, 50.0, 10);
    orderBook.processOrderDetails(102, OrderType::BUY, 48.0, 5);
    orderBook.processOrderDetails(103, OrderType::BUY, 49.0, 7);

    // Add some SELL orders
    orderBook.processOrderDetails(201, OrderType::SELL, 47.0, 6); // should match immediately with BUY @ 50
    orderBook.processOrderDetails(202, OrderType::SELL, 50.0, 4); // match with remaining BUY
    orderBook.processOrderDetails(203, OrderType::SELL, 55.0, 2); // will remain in book (too expensive)

    // Cancel an order
    orderBook.cancelOldOrder(103, OrderType::BUY, 49.0); // cancel buy order id=103

    // Show all trades that happened
    tradeBook.displayAllTrades();
    return 0;
}
