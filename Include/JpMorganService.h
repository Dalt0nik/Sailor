#pragma once

#include "TradeManager.h"
#include <string>

class JpMorganService {
public:
    JpMorganService(TradeManager& tradeManager, const std::string& api_key);
    ~JpMorganService();

    int buy_stock(const std::string& ticker, int amount, double price, const std::string& date);
    int sell_stock(const std::string& ticker, int amount, double price, const std::string& date);
    double get_total_ticker_value(const std::string& ticker);
    double get_portfolio_value();
private:
    double get_latest_price(const std::string& symbol);
    TradeManager& tradeManager;
    std::string api_key;
};
