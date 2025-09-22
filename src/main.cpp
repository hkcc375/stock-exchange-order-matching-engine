#include "../include/stock_market.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace Components;

std::chrono::system_clock::time_point parseTimeString(const std::string& timeStr)
{
    // timeStr format: "HH:MM"
    std::tm t = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&t, "%H:%M");
    if (ss.fail()) {
        throw std::runtime_error("Invalid time format: " + timeStr);
    }

    // Set date to today (or any default date)
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm today = *std::localtime(&tt);

    t.tm_mday = today.tm_mday;
    t.tm_mon = today.tm_mon;
    t.tm_year = today.tm_year;

    std::time_t time_tt = std::mktime(&t);
    return std::chrono::system_clock::from_time_t(time_tt);
}

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
