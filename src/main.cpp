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
        std::cout << "\nEnter command (NEW, CANCEL, SHOW, EXIT): ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "EXIT")
            break;
        else if (cmd == "SHOW") {
            tradeBook.displayAllTrades();
            continue;
        } else if (cmd == "CANCEL") {
            int orderId;
            std::string typeStr;
            double price;

            std::cout << "Enter order ID to cancel: ";
            std::cin >> orderId;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Enter order type (BUY/SELL): ";
            std::cin >> typeStr;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Enter order price: ";
            std::cin >> price;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            OrderType type;
            if (typeStr == "BUY")
                type = OrderType::BUY;
            else if (typeStr == "SELL")
                type = OrderType::SELL;
            else {
                std::cout << "Invalid order type.\n";
                continue;
            }

            orderBook.cancelOldOrder(orderId, type, price);
            continue;
        } else if (cmd == "NEW") {
            std::string typeStr, stock;
            double price;
            int quantity, userId;

            std::cout << "Enter Order Type (BUY/SELL): ";
            std::getline(std::cin, typeStr);

            std::cout << "Enter Stock Symbol: ";
            std::getline(std::cin, stock);

            std::cout << "Enter Price: ";
            std::cin >> price;

            std::cout << "Enter Quantity: ";
            std::cin >> quantity;

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

            orderBook.processOrderDetails(userId, type, price, quantity, stock);
            std::cout << typeStr << " order added for user " << userId << " on " << stock << "\n";
        } else {
            std::cout << "Unknown command. Use NEW, CANCEL, SHOW, or EXIT.\n";
        }
    }

    std::cout << "Exiting CLI Order Book.\n";
    return 0;
}
