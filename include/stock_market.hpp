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

namespace Components {

    enum class OrderType {BUY, SELL};

    class Order {
        // Properties of a placed Order;
        private:
            const int orderID;
            const OrderType orderType;
            OrderStatus orderStatus = OrderStatus::PLACED;
            const double price;
            int quantity;
            const std::string stockName;
            std::chrono::system_clock::time_point timestamp;
        public:
            Order(int orderId, const std::string& stockName, OrderType orderType, double price, int qty);
            int getOrderID() const;
            double getPrice() const;
            int getQuantity() const;
            OrderType getOrderType() const;
            const std::string& getStockName() const;
            void setQuantity(int qty);
            std::chrono::system_clock::time_point getTimestamp() const;
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
            std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
        public:
            Trade(int tradeId, const std::string& stockName, int buyOrderId, int sellOrderId, double price, int qty);
            int getTradeID() const;
            const std::string& getStockName() const;
            int getMatchedTradeBuyOrderID() const;
            int getMatchedTradeSellOrderID() const;
            double getTradePrice() const;
            int getTradeQuantity() const;
            std::chrono::system_clock::time_point getTimestamp() const;
    };

    class TradeBook {
        // Maintains all successful trades recorded by the system;
        private:
            static int totalTrades;
            std::vector<Trade> trades{};
        public:
            void displayAllTrades() const;
            void recordTrade(const int buyOrderId, const int sellOrderId, const double price, const int qty);
            // void queryTrade(const Trade& t) const;
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
        public:
            OrderBook(TradeBook& tb);
            void viewPlacedOrderDetails(const Order& o) const;
            void processOrderDetails(const string& stockName, const OrderType orderType, const double price, const int quantity);    };
}

std::chrono::system_clock::time_point parseTimeString(const std::string& timeStr);

#endif