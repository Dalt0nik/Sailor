#pragma once

#include "TradeManager.h"
#include "TransactionDTO.h"
class JpMorganService {
public:
    JpMorganService(TradeManager& tradeManager);
    ~JpMorganService();
    
    void buy_stock(const std::string& ticker, int amount, double price, const std::string& date);
    void sell_stock(const std::string& ticker, int amount, const std::string& date);
    double get_total_ticker_value(const std::string& ticker);
    double get_portfolio_value();
private:
    
    TradeManager& tradeManager;
}