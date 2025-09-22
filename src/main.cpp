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
    std::cout << "  BUY <userId> <price> <quantity>\n";
    std::cout << "  SELL <userId> <price> <quantity>\n";
    std::cout << "  CANCEL <orderId> <BUY|SELL> <price>\n";
    std::cout << "  SHOW   -> display all trades\n";
    std::cout << "  EXIT   -> quit\n";

    std::string line;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "BUY") {
            int userId, quantity;
            double price;
            ss >> userId >> price >> quantity;
            if (ss.fail()) {
                std::cout << "Invalid BUY command format.\n";
                continue;
            }
            orderBook.processOrderDetails(userId, OrderType::BUY, price, quantity);
            std::cout << "BUY order added for user " << userId << "\n";
        } else if (cmd == "SELL") {
            int userId, quantity;
            double price;
            ss >> userId >> price >> quantity;
            if (ss.fail()) {
                std::cout << "Invalid SELL command format.\n";
                continue;
            }
            orderBook.processOrderDetails(userId, OrderType::SELL, price, quantity);
            std::cout << "SELL order added for user " << userId << "\n";
        } else if (cmd == "CANCEL") {
            int orderId;
            std::string typeStr;
            double price;
            ss >> orderId >> typeStr >> price;
            if (ss.fail()) {
                std::cout << "Invalid CANCEL command format.\n";
                continue;
            }
            OrderType type;
            if (typeStr == "BUY")
                type = OrderType::BUY;
            else if (typeStr == "SELL")
                type = OrderType::SELL;
            else {
                std::cout << "Invalid order type. Use BUY or SELL.\n";
                continue;
            }
            orderBook.cancelOldOrder(orderId, type, price);
        } else if (cmd == "SHOW") {
            tradeBook.displayAllTrades();
        } else if (cmd == "EXIT") {
            break;
        } else {
            std::cout << "Unknown command. Try BUY, SELL, CANCEL, SHOW, EXIT.\n";
        }
    }

    std::cout << "Exiting CLI Order Book.\n";
    return 0;
}
