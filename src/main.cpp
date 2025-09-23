#include "../include/stock_market.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace Components;

int main()
{
    TradeBook tradeBook;
    OrderBook orderBook(tradeBook);

    std::cout << "=== Welcome to CLI Order Book ===\n";
    std::cout << "Commands:\n";
    std::cout << "  NEW -> place a BUY/SELL order\n";
    std::cout << "  SHOW   -> display all trades\n";
    std::cout << "  EXIT   -> quit\n";

    bool eodCleanupDone = false;

    std::string line;
    while (true) {

        auto now = std::chrono::system_clock::now();
        auto window = getTradingWindow(now);

        if (now >= window.second && !eodCleanupDone) {
            orderBook.endOfDayCleanup();
            eodCleanupDone = true;
        } else if (now < window.first && eodCleanupDone) {
            eodCleanupDone = false;
        }

        std::cout << "\nEnter command (NEW, SHOW, EXIT): ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "EXIT") {
            orderBook.endOfDayCleanup();
            break;
        } else if (cmd == "SHOW") {
            tradeBook.displayAllTrades();
            continue;
        } else if (cmd == "NEW") {
            std::string typeStr, stock, timeStr;
            double price;
            int quantity;

            std::cout << "Enter Order Type (BUY/SELL): ";
            std::getline(std::cin, typeStr);

            std::cout << "Enter Stock Symbol: ";
            std::getline(std::cin, stock);

            std::cout << "Enter Price: ";
            std::cin >> price;

            std::cout << "Enter Quantity: ";
            std::cin >> quantity;

            std::cout << "Enter Time: ";
            std::cin >> timeStr;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            OrderType type;
            if (typeStr == "BUY")
                type = OrderType::BUY;
            else if (typeStr == "SELL")
                type = OrderType::SELL;
            else {
                std::cout << "Invalid order type. Must be BUY or SELL.\n";
                continue;
            }

            auto ts = parseTimeString(timeStr);
            orderBook.processOrderDetails(stock, type, price, quantity, ts);
        } else {
            std::cout << "Unknown command. Use NEW, SHOW, or EXIT.\n";
        }
    }

    std::cout << "Exiting CLI Order Book.\n";
    return 0;
}
