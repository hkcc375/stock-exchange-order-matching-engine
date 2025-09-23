#include "../include/stock_market.hpp"

namespace Components {

int OrderBook::totalOrders = 1;
int TradeBook::totalTrades = 1;

Order::Order(int orderId, const std::string& stockName, OrderType orderType, double price, int qty)
    : orderID(orderId)
    , stock(stockName)
    , orderType(orderType)
    , price(price)
    , quantity(qty)
{
}

Trade::Trade(int tradeId, int buyOrderId, int sellOrderId, double price, int qty)
    : tradeID(tradeId)
    , buyOrderID(buyOrderId)
    , sellOrderID(sellOrderId)
    , price(price)
    , quantity(qty)
{
}

inline int Trade::getTradeID() const
{
    return tradeID;
}

inline int Trade::getMatchedTradeBuyOrderID() const
{
    return buyOrderID;
}

inline int Trade::getMatchedTradeSellOrderID() const
{
    return sellOrderID;
}

inline double Trade::getTradePrice() const
{
    return price;
}

inline const std::string& Trade::getStockName() const
{
    return stockName;
}

inline int Trade::getTradeQuantity() const
{
    return quantity;
}

inline std::chrono::system_clock::time_point Trade::getTimestamp() const
{
    return timestamp;
}

inline int Order::getOrderID() const
{
    return orderID;
}

inline const std::string& Order::getStock() const
{
    return stock;
}

inline double Order::getPrice() const
{
    return price;
}

inline int Order::getQuantity() const
{
    return quantity;
}

inline OrderType Order::getOrderType() const
{
    return orderType;
}

inline OrderStatus Order::getOrderStatus() const
{
    return orderStatus;
}

inline void Order::setQuantity(int qty)
{
    quantity = qty;
}

inline void Order::setOrderStatus(OrderStatus status)
{
    orderStatus = status;
}

inline std::chrono::system_clock::time_point Order::getTimestamp() const
{
    return timestamp;
}

OrderBook::OrderBook(TradeBook& tb)
    : tradeBook(tb)
{
}

void TradeBook::recordTrade(const std::string& stockName, const int buyOrderId, const int sellOrderId, const double price, const int qty)
{
    const Trade t(TradeBook::totalTrades++, stockName, buyOrderId, sellOrderId, price, qty);
    trades.push_back(t);

    // Record time at which Trade was added to TradeBook;
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::cout << "Trade added at " << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << std::endl;
}

void TradeBook::displayAllTrades() const
{
    for (auto trade : trades) {
        auto tp = trade.getTimestamp();
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::cout << "[TRADE] "
                  << "id = " << trade.getTradeID()
                  << "stockName = " << trade.getStockName()
                  << " buyOrderID = " << trade.getMatchedTradeBuyOrderID()
                  << " sellOrderID = " << trade.getMatchedTradeSellOrderID()
                  << " Qty = " << trade.getTradeQuantity()
                  << " Price = " << trade.getTradePrice()
                  << " ts = " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl;
    }
}

void OrderBook::matchBuy(Order& buyOrder)
{
    while (buyOrder.getQuantity() > 0 && !sellOrders.empty()) {
        auto sellIt = sellOrders.begin();
        double minSellPrice = sellIt->first;

        // buyOrder's price >= minSellPrice;
        if (buyOrder.getPrice() < minSellPrice)
            break;

        auto& sellOrdersLst = sellIt->second;
        Order& sellOrder = sellOrdersLst.front();

        int quantity = std::min(sellOrder.getQuantity(), buyOrder.getQuantity());
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);

        // A trade is generated here;
        tradeBook.recordTrade(buyOrder.getOrderID(), sellOrder.getOrderID(), sellOrder.getPrice(), quantity);

        if (sellOrder.getQuantity() == 0) {
            sellOrdersLst.pop_front();
            if (sellOrdersLst.empty())
                sellOrders.erase(sellIt);
        }
    }
}

void OrderBook::matchSell(Order& sellOrder)
{
    while (sellOrder.getQuantity() > 0 && !buyOrders.empty()) {
        auto buyIt = buyOrders.begin();
        double maxBuyPrice = buyIt->first;

        // sellOrder's price <= maxBuyPrice;
        if (sellOrder.getPrice() > maxBuyPrice)
            break;

        auto& buyOrdersLst = buyIt->second;
        Order& buyOrder = buyOrdersLst.front();

        int quantity = std::min(buyOrder.getQuantity(), sellOrder.getQuantity());
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);

        // A trade is generated here;
        tradeBook.recordTrade(buyOrder.getOrderID(), sellOrder.getOrderID(), buyOrder.getPrice(), quantity);

        if (buyOrder.getQuantity() == 0) {
            buyOrdersLst.pop_front();
            if (buyOrdersLst.empty())
                buyOrders.erase(buyIt);
        }
    }
}

void OrderBook::processOrderDetails(const string& stockName, const OrderType orderType, const double price, const int quantity)
{
    // Only if quantity and price are positive, then an Order is created;
    if (quantity > 0 && price > 0) {
        Order o(totalOrders++, stockName, orderType, price, quantity);

        /* Note : Here, matchOrders returns a boolean true/false, if the incoming order matched with an existing
           order; Their quantities are updated and a Trade object is generated; else, the order object is simply
           inserted into the order book;
        */
        if (orderType == OrderType::BUY) {
            matchBuy(o);
            if (o.getQuantity() > 0) {
                buyOrders[price].push_back(o);
            }
        } else {
            matchSell(o);
            if (o.getQuantity() > 0) {
                sellOrders[price].push_back(o);
            }
        }
    }
}

/*
 * Below function cancels an order that has orderId as its id, orderType as its type and orderPrice as its price;
 * If such an order is not present, its prints "Invalid Order Cancellation";
 */
void OrderBook::cancelOldOrder(const int orderId, const OrderType orderType, const double orderPrice)
{
    if (orderType == OrderType::BUY) {
        auto it = buyOrders.find(orderPrice);
        if (it != buyOrders.end()) {
            auto& orderLst = it->second;
            // Iterate through the list of orders and remove the order whose id is orderId;
            for (auto lit = orderLst.begin(); lit != orderLst.end(); lit++) {
                if (lit->getOrderID() == orderId) {
                    orderLst.erase(lit);
                    if (orderLst.empty())
                        buyOrders.erase(it);
                    std::cout << "Cancelled BUY Order " << orderId << std::endl;
                    return;
                }
            }
        }
        std::cout << "Invalid BUY Order Cancellation" << std::endl;
    } else {
        auto it = sellOrders.find(orderPrice);
        if (it != sellOrders.end()) {
            auto& orderLst = it->second;
            // Iterate through the list of orders and remove the order whose id is orderId;
            for (auto lit = orderLst.begin(); lit != orderLst.end(); lit++) {
                if (lit->getOrderID() == orderId) {
                    orderLst.erase(lit);
                    if (orderLst.empty())
                        sellOrders.erase(it);
                    std::cout << "Cancelled SELL Order " << orderId << std::endl;
                    return;
                }
            }
        }
        std::cout << "Invalid SELL Order Cancellation" << std::endl;
    }
}

}

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