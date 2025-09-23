#include "../include/stock_market.hpp"

namespace Components {

int OrderBook::totalOrders = 1;
int TradeBook::totalTrades = 1;

Order::Order(int orderId, const std::string& stockName, OrderType orderType, double price, int qty, std::chrono::system_clock::time_point time)
    : orderID(orderId)
    , stockName(stockName)
    , orderType(orderType)
    , price(price)
    , quantity(qty)
    , timestamp(time)
{
}

Trade::Trade(int tradeId, const std::string& stockName, int buyOrderId, int sellOrderId, double price, int qty, std::chrono::system_clock::time_point timestamp)
    : tradeID(tradeId)
    , stockName(stockName)
    , buyOrderID(buyOrderId)
    , sellOrderID(sellOrderId)
    , price(price)
    , quantity(qty)
    , timestamp(timestamp)
{
}

OrderBook::OrderBook(TradeBook& tb)
    : tradeBook(tb)
{
}

void TradeBook::recordTrade(const std::string& stockName, const int buyOrderId, const int sellOrderId, const double price, const int qty, const std::chrono::system_clock::time_point timestamp)
{
    const Trade t(TradeBook::totalTrades++, stockName, buyOrderId, sellOrderId, price, qty, timestamp);
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
        std::cout << "[TRADE]"
                  << " id = " << trade.getTradeID()
                  << " stockName = " << trade.getStockName()
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
        auto timestamp = buyOrder.getTimestamp() > sellOrder.getTimestamp() ? buyOrder.getTimestamp() : sellOrder.getTimestamp();
        tradeBook.recordTrade(buyOrderStockName, buyOrder.getOrderID(), sellOrder.getOrderID(), minSellPrice, quantity, timestamp);

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
        auto timestamp = buyOrder.getTimestamp() > sellOrder.getTimestamp() ? buyOrder.getTimestamp() : sellOrder.getTimestamp();
        tradeBook.recordTrade(sellOrderStockName, buyOrder.getOrderID(), sellOrder.getOrderID(), maxBuyPrice, quantity, timestamp);

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

void OrderBook::processOrderDetails(const std::string& stockName, const OrderType orderType, const double price, const int quantity, const std::chrono::system_clock::time_point time)
{
    // Reject Out-Of-Hours orders;
    if (!isWithinTradingHours(time)) {
        std::cout << "Order rejected: outside trading hours\n";
        return;
    }

    // Only if quantity and price are positive, then an Order is created;
    if (quantity > 0 && price > 0) {
        Order o(totalOrders++, stockName, orderType, price, quantity, time);
        viewPlacedOrderDetails(o);

        /* Note : Here, matchOrders returns a boolean true/false, if the incoming order matched with an existing
           order; Their quantities are updated and a Trade object is generated; else, the order object is simply
           inserted into the order book;
        */
        if (orderType == OrderType::BUY) {
            matchBuy(o);
            if (o.getQuantity() > 0) {
                buyOrders[stockName][price].push_back(o);
            }
        } else {
            matchSell(o);
            if (o.getQuantity() > 0) {
                sellOrders[stockName][price].push_back(o);
            }
        }
    }
}

void OrderBook::viewPlacedOrderDetails(const Order& o) const
{
    auto tp = o.getTimestamp();
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::cout << "[ORDER]"
              << " id = " << o.getOrderID()
              << " stockName = " << o.getStockName()
              << " Qty = " << o.getQuantity()
              << " Price = " << o.getPrice()
              << " ts = " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl;
}

void OrderBook::endOfDayCleanup()
{
    auto cancelSide = [&](auto& outerMap, const char* sideName) {
        for (auto outerMapIt = outerMap.begin(); outerMapIt != outerMap.end();) {
            std::string stockName = outerMapIt->first;
            auto& innerMap = outerMapIt->second;

            for (auto innerMapIt = innerMap.begin(); innerMapIt != innerMap.end();) {
                double price = innerMapIt->first;
                auto& orderLst = innerMapIt->second;

                for (auto lit = orderLst.begin(); lit != orderLst.end();) {
                    Order& order = *lit;

                    std::cout << "[EOD CANCEL] side = " << sideName
                              << " stock = " << stockName
                              << " orderId = " << order.getOrderID()
                              << " qty = " << order.getQuantity()
                              << " price = " << order.getPrice()
                              << std::endl;

                    lit = orderLst.erase(lit);
                }

                if (orderLst.empty()) {
                    innerMapIt = innerMap.erase(innerMapIt);
                } else {
                    ++innerMapIt;
                }
            }

            if (innerMap.empty()) {
                outerMapIt = outerMap.erase(outerMapIt);
            } else {
                ++outerMapIt;
            }
        }
    };

    cancelSide(buyOrders, "BUY");
    cancelSide(sellOrders, "SELL");

    std::cout << "All orders removed\n"
              << std::endl;
}

}

bool isWithinTradingHours(const std::chrono::system_clock::time_point& now)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm local = *std::localtime(&tt);
    int hh = local.tm_hour;
    int mm = local.tm_min;

    // trading window: 09:15 <= time < 15:30
    bool afterOpen = (hh > 9) || (hh == 9 && mm >= 15);
    bool beforeClose = (hh < 15) || (hh == 15 && mm < 30);
    return afterOpen && beforeClose;
}

std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>
getTradingWindow(const std::chrono::system_clock::time_point& anyTimeOnDay)
{
    std::time_t tt = std::chrono::system_clock::to_time_t(anyTimeOnDay);
    std::tm day = *std::localtime(&tt);

    std::tm open_tm = day;
    open_tm.tm_hour = 9;
    open_tm.tm_min = 15;
    open_tm.tm_sec = 0;

    std::tm close_tm = day;
    close_tm.tm_hour = 15;
    close_tm.tm_min = 30;
    close_tm.tm_sec = 0;

    auto open_tp = std::chrono::system_clock::from_time_t(std::mktime(&open_tm));
    auto close_tp = std::chrono::system_clock::from_time_t(std::mktime(&close_tm));
    return { open_tp, close_tp };
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
