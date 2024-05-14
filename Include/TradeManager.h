#ifndef TRADE_MANAGER_H
#define TRADE_MANAGER_H

#include "DatabaseManager.h"
#include <string>
#include <vector>
#include <map>

class TradeManager {
public:
    TradeManager(DatabaseManager& dbManager);
    ~TradeManager();

    void insertTradeHistory(const std::string& tx_type, const std::string& ticker, int amount, double price, const std::string& date);
    void insertCurrentAssets(const std::string& ticker, int amount, double average_price);
    void updateCurrentAssets(const std::string& ticker, int amount, double average_price);
    std::vector<std::map<std::string, std::string>> selectAllFromCurrentAssets();
    std::vector<std::map<std::string, std::string>> selectAllFromTradeHistory();

private:
    DatabaseManager& dbManager;
};

#endif // TRADE_MANAGER_H
