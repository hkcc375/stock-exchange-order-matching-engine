#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <map>
#include <queue>

namespace Components {

    enum class OrderType {BUY, SELL};
    enum class OrderStatus {PLACED, PARTIALLY_FILLED, FILLED, CANCELLED};

    class Order {
        // Properties of a placed Order;
        private:
            int orderID;
            int userID;
            OrderType orderType;
            OrderStatus orderStatus = OrderStatus::PLACED;
            double price;
            int quantity;
            chrono::system_clock::time_point timestamp = chrono::system_clock::now();
        public:
            Order(int orderId, int userId, OrderType orderType, double price, int qty);
            const int& getOrderID() const;
            const int& getUserID() const;
            const double& getPrice() const;
            const int& getQuantity() const;
            OrderType getOrderType() const;
            OrderStatus getOrderStatus() const;
            chrono::system_clock::time_point getTimestamp() const;
    };

    class Trade {
        // Properties of a successful Trade;
        private:
            int tradeID;
            int buyOrderID;
            int sellOrderID;
            double price;
            int quantity;
            chrono::system_clock::time_point timestamp = chrono::system_clock::now();
        public:
            Trade(int tradeId, int buyOrderId, int sellOrderId, double price, int qty)
            const int& getTradeID() const;
            const int& getMatchedTradeBuyOrderID() const;
            const int& getMatchedTradeSellOrderID() const;
            const double& getTradePrice() const;
            const int& getTradeQuantity() const;
            chrono::system_clock::time_point getTimestamp() const;
    };

    class TradeBook {
        // Maintains all successful trades recorded by the system;
        private:
            vector<Trade> trades{};
        public:
            void displayAllTrades() const;
            void recordTrade(const Trade& t);
            // void queryTrade(const Trade& t) const;
    };

    class OrderBook {
        private:
            // Price -> Queue of Orders (ordered by Timestamp);
            map<double, queue<Order>> buyOrders{};
            map<double, queue<Order>> sellOrders{};
        public:
            void matchOrders();
            void processNewOrder(const Order& o);
            void cancelOldOrder(const Order& o); 
    };
}

#endif