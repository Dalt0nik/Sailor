#pragma once

#include "TradeRepository.h"
#include <string>

class AssetService {
public:
    AssetService(TradeRepository& tradeRepository, const std::string& api_key);
    ~AssetService();

    int buy_stock(const std::string& ticker, int amount, double price, const std::string& date);
    int sell_stock(const std::string& ticker, int amount, double price, const std::string& date);
    double get_total_ticker_value(const std::string& ticker);
    double get_portfolio_value();
private:
    double get_latest_price(const std::string& symbol);
    TradeRepository& tradeRepository;
    std::string api_key;
};
