#pragma once

#include "TradeRepository.h"
#include <string>
#include <vector>

class AssetService {
public:
    AssetService(TradeRepository& tradeRepository, const std::string& api_key);
    ~AssetService();

    int buy_stock(const std::string& ticker, int amount, double price, const std::string& date);
    int sell_stock(const std::string& ticker, int amount, double price, const std::string& date);
    double get_total_ticker_value(const std::string& ticker);
    double get_portfolio_value();
    
    double get_all_expenses_by_ticker(const std::string &ticker); // we can do these functions private
    double get_all_sells_by_ticker(const std::string &ticker);    // and create get_zarabotok method
    std::vector<std::string> GetAllTickers();
    double calculate_ticker_profit(const std::string &ticker);
private:
    double get_latest_price(const std::string& symbol);
    TradeRepository& tradeRepository;
    std::string api_key;
};
