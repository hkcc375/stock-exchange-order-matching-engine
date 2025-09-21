#include "../include/stock_market.hpp"

namespace Components {

Order::Order(int orderId, int userId, OrderType orderType, double price, int qty)
    : orderID(orderId)
    , userID(userId)
    , orderType(orderType)
    , price(price)
    , quantity(qty)
{
}

int tradeID;
int buyOrderID;
int sellOrderID;
double price;
int quantity;

Trade::Trade(int tradeId, int buyOrderId, int sellOrderId, double price, int qty)
    : tradeID(tradeId)
    , buyOrderID(buyOrderId)
    , sellOrderID(sellOrderId)
    , price(price)
    , quantity(qty)
{
}

TradeBook::recordTrade(const Trade& t)
{
    trades.push_back(t);

    // Record time at which Trade was added to TradeBook;
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    cout << "Trade added to Trade book at " << put_time(localtime(&now_time), "%Y-%m-%d %H:%M:%S") << endl;
}

}
