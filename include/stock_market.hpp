#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

namespace Components {

    enum class OrderType {BUY, SELL};
    enum class OrderStatus {PLACED, PARTIALLY_FILLED, FILLED, CANCELLED};

    class Order {
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
        private:
            int tradeID;
            int buyOrderID;
            int sellOrderID;
            double price;
            int quantity;
            chrono::system_clock::time_point timestamp;
        public:
    };

}

#endif