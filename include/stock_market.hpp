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
            std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
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
            std::chrono::system_clock::time_point getTimestamp() const;
    };

    class Trade {
        // Properties of a successful Trade;
        private:
            const int tradeID;
            const int buyOrderID;
            const int sellOrderID;
            const double price;
            const int quantity;
            std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
        public:
            Trade(int tradeId, int buyOrderId, int sellOrderId, double price, int qty);
            int getTradeID() const;
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
            // Price -> List of Orders (ordered by Timestamp);
            std::map<double, std::list<Order>, std::greater<double>> buyOrders{};
            std::map<double, std::list<Order>> sellOrders{};
            void matchBuy(Order& o);
            void matchSell(Order& o);
        public:
            OrderBook(TradeBook& tb);
            void processOrderDetails(const int userId, const OrderType orderType, const double price, const int quantity);
            void cancelOldOrder(const int orderId, const OrderType orderType, const double orderPrice); 
    };

}

#endif