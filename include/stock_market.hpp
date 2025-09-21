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
        // Properties of an Order placed;
        private:
            int orderID;
            int userID;
            OrderType orderType;
            OrderStatus orderStatus;
            double price;
            int quantity;
            chrono::system_clock::time_point timestamp;
        public:
    };

    class Trade {
        // Properties of a successful Trade;
        private:
            int tradeID;
            int buyOrderID;
            int sellOrderID;
            double price;
            int quantity;
            chrono::system_clock::time_point timestamp;
        public:
    };

    class TradeBook {
        // Maintains all successful trades recorded by the system;
        private:
            vector<Trade> trades;
        public:
            void displayAllTrades() const;
            void recordTrade(Trade& t);
            void queryTrade(Trade& t) const;
    };

    class OrderBook {
        private:
            // Price -> Queue of Orders (ordered by Timestamp);
            map<double, queue<Order>> buyOrders;
            map<double, queue<Order>> sellOrders;
        public:
            void matchOrders();
            void processNewOrder(const Order& o);
            void cancelOldOrder(const Order& o); 
    };
}

#endif