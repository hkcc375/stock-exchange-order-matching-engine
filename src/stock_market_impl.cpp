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

void OrderBook::processOrderDetails(const int userId, const OrderType orderType, const double price, int quantity)
{
    if (quantity > 0 && price > 0) {
        Order o(totalOrders++, userId, orderType, price, quantity);
        if (orderType == OrderType::BUY) {
            buyOrders[price].push(o);
        } else {
            sellOrders[price].push(o);
        }
    }
}

}
