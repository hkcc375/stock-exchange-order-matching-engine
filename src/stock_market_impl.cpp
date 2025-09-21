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

}
