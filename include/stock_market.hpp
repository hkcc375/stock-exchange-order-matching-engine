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

    // I now do not see the point of maintaining OrderStatus;
    enum class OrderStatus {PLACED, PARTIALLY_FILLED, FILLED, CANCELLED};

    class Order {
        // Properties of a placed Order;
        private:
            const int orderID;
            const int userID;
            const OrderType orderType;
            OrderStatus orderStatus = OrderStatus::PLACED;
            const double price;
            int quantity;
            chrono::system_clock::time_point timestamp = chrono::system_clock::now();
        public:
            Order(int orderId, int userId, OrderType orderType, double price, int qty);
            int getOrderID() const;
            int getUserID() const;
            double getPrice() const;
            int getQuantity() const;
            OrderType getOrderType() const;
            OrderStatus getOrderStatus() const;
            void setQuantity(int qty);
            void setOrderStatus(OrderStatus status);
            chrono::system_clock::time_point getTimestamp() const;
    };

    class Trade {
        // Properties of a successful Trade;
        private:
            const int tradeID;
            const int buyOrderID;
            const int sellOrderID;
            const double price;
            const int quantity;
            chrono::system_clock::time_point timestamp = chrono::system_clock::now();
        public:
            Trade(int tradeId, int buyOrderId, int sellOrderId, double price, int qty)
            int getTradeID() const;
            int getMatchedTradeBuyOrderID() const;
            int getMatchedTradeSellOrderID() const;
            double getTradePrice() const;
            int getTradeQuantity() const;
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
            // totalOrders is currently not thread-safe;
            static int totalOrders;
            // Price -> Queue of Orders (ordered by Timestamp);
            map<double, list<Order>, greater<double>> buyOrders{};
            map<double, list<Order>> sellOrders{};
        public:
            void matchOrders(Order& o);
            void processOrderDetails(const int userId, const OrderType orderType, const double price, int quantity);
            void cancelOldOrder(const int orderId); 
    };

    class OrderComparator {
        public:
            bool operator()(const Order& a, const Order& b) const;
    };
}

#endif