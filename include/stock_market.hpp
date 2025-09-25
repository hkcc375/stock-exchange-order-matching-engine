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
#include <atomic>
#include <mutex>
#include <fstream>
#include <stdexcept>

namespace Components {

    enum class OrderType {BUY, SELL};

    class Order {
        private:
            const std::string orderID;
            const OrderType orderType;
            const double price;
            int quantity;
            const std::string stockName;
            std::chrono::system_clock::time_point timestamp;
        public:
            Order(std::string orderId, const std::string& stockName, OrderType orderType, double price, int qty, std::chrono::system_clock::time_point time);
            std::string getOrderID() const {
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
        private:
            const int tradeID;
            const std::string stockName;
            const std::string buyOrderID;
            const std::string sellOrderID;
            const double price;
            const int quantity;
            std::chrono::system_clock::time_point timestamp;
        public:
            Trade(int tradeId, const std::string& stockName, std::string buyOrderId, std::string sellOrderId, double price, int qty, std::chrono::system_clock::time_point timestamp);
            int getTradeID() const {
                return tradeID;
            }
            const std::string& getStockName() const {
                return stockName;
            }
            std::string getMatchedTradeBuyOrderID() const {
                return buyOrderID;
            }
            std::string getMatchedTradeSellOrderID() const {
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
        private:
            mutable std::mutex trades_mtx_;
            std::atomic<int> totalTrades = 1;
            std::vector<Trade> trades{};
        public:
            void displayAllTrades() const;
            void recordTrade(const std::string& stockName, const std::string& buyOrderId, const std::string& sellOrderId, const double price, const int qty, const std::chrono::system_clock::time_point timestamp);
    };

    class OrderBook {
        private:
            mutable std::mutex orders_mtx_;
            // Dependency Injection;
            TradeBook& tradeBook;

            std::atomic<int> totalOrders = 1;
            // stockName -> Price -> List of Orders (ordered by Timestamp);
            std::map<std::string, std::map<double, std::list<Order>, std::greater<double>>> buyOrders{};
            std::map<std::string, std::map<double, std::list<Order>>> sellOrders{};

            template <typename PriceMap, typename Predicate>
            void matchAgainst(Order& incomingOrder, PriceMap& oppositePriceMap, Predicate priceAcceptable);
            void matchBuy(Order& o);
            void matchSell(Order& o);
        public:
            OrderBook(TradeBook& tb);
            void processOrderDetails(const std::string& orderId, const std::string& stockName, const OrderType orderType, const double price, const int quantity, const std::chrono::system_clock::time_point time);
            void endOfDayCleanup();    
    };

    std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> getTradingHours(const std::string& filename);
    std::chrono::system_clock::time_point parseTimeString(const std::string& timeStr);
}


#endif