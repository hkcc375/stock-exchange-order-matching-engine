#include "../include/stock_market.hpp"

namespace Components {

int OrderBook::totalOrders = 1;

Order::Order(int orderId, int userId, OrderType orderType, double price, int qty)
    : orderID(orderId)
    , userID(userId)
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

inline const int& Trade::getTradeID() const
{
    return tradeID;
}

inline const int& Trade::getMatchedTradeBuyOrderID() const
{
    return buyOrderID;
}

inline const int& Trade::getMatchedTradeSellOrderID() const
{
    return sellOrderID;
}

inline const double& Trade::getTradePrice() const
{
    return price;
}

inline const int& Trade::getTradeQuantity() const
{
    return quantity;
}

inline chrono::system_clock::time_point Trade::getTimestamp() const
{
    return timestamp;
}

inline const int& Order::getOrderID() const
{
    return orderID;
}

inline const int& Order::getUserID() const
{
    return userID;
}

inline const double& Order::getPrice() const
{
    return price;
}

inline const int& Order::getQuantity() const
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

inline chrono::system_clock::time_point Order::getTimestamp() const
{
    return timestamp;
}

void TradeBook::recordTrade(const Trade& t)
{
    trades.push_back(t);

    // Record time at which Trade was added to TradeBook;
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    cout << "Trade added at " << put_time(localtime(&now_time), "%Y-%m-%d %H:%M:%S") << endl;
}

void TradeBook::displayAllTrades() const
{
    for (auto trade : trades) {
        auto tp = trade.getTimestamp();
        time_t t = chrono::system_clock::to_time_t(tp);
        cout << "[TRADE] "
             << "id =" << trade.getTradeID()
             << " buyOrderID = " << trade.getBuyOrderID()
             << " sellOrderID = " << trade.getSellOrderID()
             << " Qty = " << trade.getQuantity()
             << " Price = " << trade.getTradePrice()
             << " ts = " << trade.getTimestampString() << std::endl;
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

        int quantity = min(sellOrder.getQuantity(), buyOrder.getQuantity());
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);

        if (sellOrder.getQuantity() == 0) {
            sellOrdersLst.pop_front(sellOrder);
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

        int quantity = min(buyOrder.getQuantity(), sellOrder.getQuantity());
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);

        if (buyOrder.getQuantity() == 0) {
            buyOrdersLst.pop_front(buyOrder);
            if (buyOrdersLst.empty())
                buyOrders.erase(buyIt);
        }
    }
}

void OrderBook::processOrderDetails(const int userId, const OrderType orderType, const double price, const int quantity)
{
    // Only if quantity and price are positive, then an Order is created;
    if (quantity > 0 && price > 0) {
        Order o(totalOrders++, userId, orderType, price, quantity);

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
                    cout << "Cancelled BUY Order " << orderId << endl;
                    return;
                }
            }
        }
        cout << "Invalid BUY Order Cancellation" << endl;
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
                    cout << "Cancelled SELL Order " << orderId << endl;
                    return;
                }
            }
        }
        cout << "Invalid SELL Order Cancellation" << endl;
    }
}
