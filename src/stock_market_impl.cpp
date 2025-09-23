#include "../include/stock_market.hpp"

namespace Components {

int OrderBook::totalOrders = 1;
int TradeBook::totalTrades = 1;

Order::Order(int orderId, const std::string& stockName, OrderType orderType, double price, int qty)
    : orderID(orderId)
    , stockName(stockName)
    , orderType(orderType)
    , price(price)
    , quantity(qty)
{
}

Trade::Trade(int tradeId, const std::string& stockName, int buyOrderId, int sellOrderId, double price, int qty)
    : tradeID(tradeId)
    , stockName(stockName)
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

inline const std::string& Order::getStockName() const
{
    return stockName;
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
    // Finds the stockName of buyOrder in sellOrderBook;
    const std::string& buyOrderStockName = buyOrder.getStockName();
    auto sellIter = sellOrders.find(buyOrderStockName);

    // While there exists a stock in sellOrders with the given stockName &&
    // the incoming buyOrder's quantity is positive &&
    // there exist price levels for this stock;
    while (sellIter != sellOrders.end() && buyOrder.getQuantity() > 0 && !sellIter->second.empty()) {

        // Obtains the sellPriceMap for a particular stockName;
        auto& sellPriceMap = sellIter->second;

        // Obtains the sellOrder with minimum selling price;
        auto sellIt = sellPriceMap.begin();
        double minSellPrice = sellIt->first;

        // CONDITION for Trade : buyOrder's price >= minSellPrice;
        if (buyOrder.getPrice() < minSellPrice)
            break;

        // Obtain the sellOrderList, which is ordered on the basis of timestamp;
        auto& sellOrdersLst = sellIt->second;
        Order& sellOrder = sellOrdersLst.front();

        // Update the quantities of buyOrder and sellOrder;
        int quantity = std::min(sellOrder.getQuantity(), buyOrder.getQuantity());
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);

        // A trade is generated here;
        tradeBook.recordTrade(buyOrder.getOrderID(), sellOrder.getOrderID(), minSellPrice, quantity);

        // If resting order's quantity becomes 0;
        if (sellOrder.getQuantity() == 0) {
            sellOrdersLst.pop_front();
            if (sellOrdersLst.empty())
                sellPriceMap.erase(sellIt);
        }

        // If ever, the stock has no more price levels, then cleanup;
        if (sellPriceMap.empty()) {
            sellOrders.erase(sellIter);
            // No more trades can now be executed for this stock;
            break;
        }
    }
}

void OrderBook::matchSell(Order& sellOrder)
{
    // Finds the stockName of sellOrder in buyOrderBook;
    const std::string& sellOrderStockName = sellOrder.getStockName();
    auto buyIter = buyOrders.find(sellOrderStockName);

    // While there exists a stock in buyOrders with the given stockName &&
    // the incoming sellOrder's quantity is positive &&
    // there exist price levels for this stock;
    while (buyIter != buyOrders.end() && sellOrder.getQuantity() > 0 && !buyIter->second.empty()) {

        // Obtain the buyPriceMap for a particular stockName;
        auto& buyPriceMap = buyIter->second;

        // Obtain the buyOrder with maximum buying price;
        auto buyIt = buyPriceMap.begin();
        double maxBuyPrice = buyIt->first;

        // CONDITION FOR TRADE : sellOrder's price <= maxBuyPrice;
        if (sellOrder.getPrice() > maxBuyPrice)
            break;

        // Obtain the buyOrderList, which is ordered on the basis of timestamp;
        auto& buyOrdersLst = buyIt->second;
        Order& buyOrder = buyOrdersLst.front();

        // Update the quantities of buyOrder and sellOrder;
        int quantity = std::min(buyOrder.getQuantity(), sellOrder.getQuantity());
        buyOrder.setQuantity(buyOrder.getQuantity() - quantity);
        sellOrder.setQuantity(sellOrder.getQuantity() - quantity);

        // A trade is generated here;
        tradeBook.recordTrade(buyOrder.getOrderID(), sellOrder.getOrderID(), maxBuyPrice, quantity);

        // If resting order's quantity becomes 0;
        if (buyOrder.getQuantity() == 0) {
            buyOrdersLst.pop_front();
            if (buyOrdersLst.empty())
                buyPriceMap.erase(buyIt);
        }

        // If ever, the stock has no more price levels, then cleanup;
        if (buyPriceMap.empty()) {
            buyOrders.erase(buyIter);
            // No more trades can now be executed for this stock;
            break;
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