#include "stock_market.hpp"

namespace Components {

// Fn : Constructor fn for Order class;
Order::Order(std::string orderId, const std::string& stockName, OrderType orderType, double price, int qty, std::chrono::system_clock::time_point time)
    : orderID(orderId)
    , orderType(orderType)
    , price(price)
    , quantity(qty)
    , stockName(stockName)
    , timestamp(time)
{
}

// Fn : Constructor fn for Trade class;
Trade::Trade(int tradeId, const std::string& stockName, std::string buyOrderId, std::string sellOrderId, double price, int qty, std::chrono::system_clock::time_point timestamp)
    : tradeID(tradeId)
    , stockName(stockName)
    , buyOrderID(buyOrderId)
    , sellOrderID(sellOrderId)
    , price(price)
    , quantity(qty)
    , timestamp(timestamp)
{
}

// Fn : Constructor fn for OrderBook class;
OrderBook::OrderBook(TradeBook& tb)
    : tradeBook(tb)
{
}

/* Fn : Constructs a Trade object t and records the Trade into TradeBook
   Inputs :
    - string stockName
    - int buyOrderId
    - int sellOrderId
    - double price
    - int qty
    - Datetime timestamp
   Output : Trade object t
*/
void TradeBook::recordTrade(const std::string& stockName, const std::string& buyOrderId, const std::string& sellOrderId, const double price, const int qty, const std::chrono::system_clock::time_point timestamp)
{
    int tradeID;
    // Critical Section - START;
    {
        std::lock_guard<std::mutex> lg(trades_mtx_);
        tradeID = TradeBook::totalTrades++;
        const Trade t(tradeID, stockName, buyOrderId, sellOrderId, price, qty, timestamp);
        trades.push_back(t);
    }
    // Critical Section - END;
}

// Fn : Displays all the trades present in TradeBook in the format : <sell-order-id> <qty> <sell-price> <buy-order-id>;
void TradeBook::displayAllTrades() const
{
    std::lock_guard<std::mutex> lg(trades_mtx_);
    for (const auto& trade : trades) {
        std::cout << trade.getMatchedTradeSellOrderID()
                  << " " << trade.getTradeQuantity()
                  << " " << trade.getTradePrice()
                  << " " << trade.getMatchedTradeBuyOrderID()
                  << "\n";
    }
}

template <typename PriceMap, typename Predicate>
void OrderBook::matchAgainst(Order& incomingOrder, PriceMap& oppositePriceMap, Predicate priceAcceptable)
{
    while (incomingOrder.getQuantity() > 0 && !oppositePriceMap.empty()) {

        auto priceIt = oppositePriceMap.begin();
        double bestPrice = priceIt->first;

        // Check if trade happens;
        if (!priceAcceptable(incomingOrder.getPrice(), bestPrice)) {
            break;
        }

        // Obtain list of all resting orders at this price;
        auto& restingOrders = priceIt->second;
        Order& resting = restingOrders.front();

        // Quantity for which the trade happend;
        int quantity = std::min(incomingOrder.getQuantity(), resting.getQuantity());

        // Update quantities of the orders;
        incomingOrder.setQuantity(incomingOrder.getQuantity() - quantity);
        resting.setQuantity(resting.getQuantity() - quantity);

        // Choose the timestamp of the latest order for the trade;
        auto ts = (incomingOrder.getTimestamp() > resting.getTimestamp())
            ? incomingOrder.getTimestamp()
            : resting.getTimestamp();

        // Record trade;
        if (incomingOrder.getOrderType() == OrderType::BUY) {
            tradeBook.recordTrade(incomingOrder.getStockName(),
                incomingOrder.getOrderID(), resting.getOrderID(),
                bestPrice, quantity, ts);
        } else {
            tradeBook.recordTrade(incomingOrder.getStockName(),
                resting.getOrderID(), incomingOrder.getOrderID(),
                bestPrice, quantity, ts);
        }

        // Remove the resting order if it is completely satisfied;
        if (resting.getQuantity() == 0) {
            restingOrders.pop_front();
            if (restingOrders.empty()) {
                oppositePriceMap.erase(priceIt);
            }
        }
    }
}

// Fn : Matches an incoming sellOrder with any buyOrder already present in buyOrderBook
void OrderBook::matchBuy(Order& incomingBuyOrder)
{
    auto it = sellOrders.find(incomingBuyOrder.getStockName());
    if (it == sellOrders.end())
        return;

    matchAgainst(incomingBuyOrder, it->second,
        [](double buyPrice, double minSellPrice) {
            return buyPrice >= minSellPrice;
        });
}

// Fn : Matches an incoming sellOrder with any buyOrder already present in buyOrderBook
void OrderBook::matchSell(Order& incomingSellOrder)
{
    auto it = buyOrders.find(incomingSellOrder.getStockName());
    if (it == buyOrders.end())
        return;

    matchAgainst(incomingSellOrder, it->second,
        [](double sellPrice, double maxBuyPrice) {
            return sellPrice <= maxBuyPrice;
        });
}

/* Fn : Processes all the details of an order
    Input :
     - string stockName
     - OrderType orderType
     - double price
     - int quantity
     - Datetime time
    Output :
     - Creates Order object if valid details given
     - Records Trade / places Order object in either Buy / Sell OrderBook
*/
void OrderBook::processOrderDetails(const std::string& orderId, const std::string& stockName, const OrderType orderType, const double price, const int quantity, const std::chrono::system_clock::time_point time)
{
    // Critical Section - START;
    {
        // Holds a mutex, so that we dont run into an concurrent accesses;
        std::lock_guard<std::mutex> lg(orders_mtx_);
        // Only if quantity and price are positive, then an Order is created;
        if (quantity > 0 && price > 0) {
            Order o(orderId, stockName, orderType, price, quantity, time);

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
        } else {
            std::cout << "Invalid Order Details" << std::endl;
        }
    }
    // Critical Section - END;
}

// Fn : An OrderBook cleanup utility that removes all remaining orders in OrderBooks after trading hours
void OrderBook::endOfDayCleanup()
{
    auto cancelSide = [&](auto& outerMap) {
        for (auto outerMapIt = outerMap.begin(); outerMapIt != outerMap.end();) {
            std::string stockName = outerMapIt->first;
            auto& innerMap = outerMapIt->second;

            for (auto innerMapIt = innerMap.begin(); innerMapIt != innerMap.end();) {
                auto& orderLst = innerMapIt->second;

                for (auto lit = orderLst.begin(); lit != orderLst.end();) {
                    Order& order = *lit;

                    // Order format : <order-id> <stock> <buy/sell> <qty> <price>;
                    std::cout << "[EOD CANCEL] " << order.getOrderID()
                              << " " << order.getStockName()
                              << " " << ((order.getOrderType() == OrderType::BUY) ? "buy" : "sell")
                              << " " << order.getQuantity()
                              << " " << order.getPrice()
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

    cancelSide(buyOrders);
    cancelSide(sellOrders);
}

/* Utility Fn : Converts timeStr string to a Datetime object and returns this object
    Input : string timeStr (in HH:MM time format)
    Output : Datetime object represented in YYYY-mm-dd HH:MM:SS format
*/
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

/* Utility Fn : Loads start and end trading time from config ini file;
    Input : Datetime object now
    Output : Pair of (Start Trading time, End Trading time);
*/
std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>
getTradingHours(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("getTradingHours: cannot open config file: " + filename);
    }

    std::string line;
    std::string open;
    std::string close;
    while (std::getline(file, line)) {
        if (line.rfind("open=", 0) == 0)
            open = line.substr(5);
        else if (line.rfind("close=", 0) == 0)
            close = line.substr(6);
    }

    std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point> tw;
    tw.first = parseTimeString(open);
    tw.second = parseTimeString(close);
    return tw;
}

}
