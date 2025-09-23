#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <vector>
#include <queue>
#include <list>
#include <iomanip>
#include <sstream>

namespace Components {

    enum class OrderType {BUY, SELL};

    class Order {
        // Properties of a placed Order;
        private:
            const int orderID;
            const OrderType orderType;
            const double price;
            int quantity;
            const std::string stockName;
            std::chrono::system_clock::time_point timestamp;
        public:
            Order(int orderId, const std::string& stockName, OrderType orderType, double price, int qty, std::chrono::system_clock::time_point time);
            int getOrderID() const {
                return orderID;
            }
            double getPrice() const {
                return price;
            }
            int getQuantity() const {
                return quantity;
            }
            OrderType getOrderType() const {
                return orderType;
            }
            const std::string& getStockName() const {
                return stockName;
            }
            void setQuantity(int qty) {
                quantity = qty;
            }
            std::chrono::system_clock::time_point getTimestamp() const {
                return timestamp;
            }
    };

    class Trade {
        // Properties of a successful Trade;
        private:
            const int tradeID;
            const std::string& stockName;
            const int buyOrderID;
            const int sellOrderID;
            const double price;
            const int quantity;
            std::chrono::system_clock::time_point timestamp;
        public:
            Trade(int tradeId, const std::string& stockName, int buyOrderId, int sellOrderId, double price, int qty, std::chrono::system_clock::time_point timestamp);
            int getTradeID() const {
                return tradeID;
            }
            const std::string& getStockName() const {
                return stockName;
            }
            int getMatchedTradeBuyOrderID() const {
                return buyOrderID;
            }
            int getMatchedTradeSellOrderID() const {
                return sellOrderID;
            }
            double getTradePrice() const {
                return price;
            }
            int getTradeQuantity() const {
                return quantity;
            }
            std::chrono::system_clock::time_point getTimestamp() const {
                return timestamp;
            }
    };

    class TradeBook {
        // Maintains all successful trades recorded by the system;
        private:
            static int totalTrades;
            std::vector<Trade> trades{};
        public:
            void displayAllTrades() const;
            void recordTrade(const std::string& stockName, const int buyOrderId, const int sellOrderId, const double price, const int qty, const std::chrono::system_clock::time_point timestamp);
            const std::vector<Trade>& getTrades() const;
    };

    class OrderBook {
        private:
            // Dependency Injection;
            TradeBook& tradeBook;
            static int totalOrders;
            // stockName -> Price -> List of Orders (ordered by Timestamp);
            std::map<std::string, std::map<double, std::list<Order>, std::greater<double>>> buyOrders{};
            std::map<std::string, std::map<double, std::list<Order>>> sellOrders{};
            void matchBuy(Order& o);
            void matchSell(Order& o);
            void viewPlacedOrderDetails(const Order& o) const;
        public:
            OrderBook(TradeBook& tb);
            void processOrderDetails(const std::string& stockName, const OrderType orderType, const double price, const int quantity, const std::chrono::system_clock::time_point time);
            void endOfDayCleanup();    
    };
}

// Utility Fn - converts given time format HH:MM  to YYYY-mm-dd HH:MM:SS;
std::chrono::system_clock::time_point parseTimeString(const std::string& timeStr);

// Utility Fn - checks if the user provided time is within trading hours;
std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>
getTradingWindow(const std::chrono::system_clock::time_point& anyTimeOnDay);

bool isWithinTradingHours(const std::chrono::system_clock::time_point& now);

#endif