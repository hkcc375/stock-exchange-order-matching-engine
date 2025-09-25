#include "stock_market.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace Components;

int main()
{
    std::string cfg = "trading_hours.ini";
    auto tradingWindow = getTradingHours(cfg);

    TradeBook tradeBook;
    OrderBook orderBook(tradeBook);

    bool eodCleanupDone = false;

    std::vector<std::string> allOrders;
    std::string line;
    // Read multiple lines until empty line
    while (true) {
        std::getline(std::cin, line);
        if (line.empty()) // empty line signals end of input
            break;
        allOrders.push_back(line);
    }

    // Process each order line
    for (const auto& orderLine : allOrders) {
        std::istringstream iss(orderLine);
        std::string orderId, timeStr, stockName, orderTypeStr;
        int qty;
        double price;
        iss >> orderId >> timeStr >> stockName >> orderTypeStr >> qty >> price;

        OrderType type = (orderTypeStr == "buy") ? OrderType::BUY : OrderType::SELL;

        auto ts = parseTimeString(timeStr);
        if (ts < tradingWindow.first || (ts > tradingWindow.second && !eodCleanupDone)) {
            std::cout << "Order rejected : outside trading hours\n";
            continue;
        }

        orderBook.processOrderDetails(orderId, stockName, type, price, qty, ts);
    }

    tradeBook.displayAllTrades();
    orderBook.endOfDayCleanup();

    return 0;
}
