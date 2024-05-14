#ifndef TRADE_MANAGER_H
#define TRADE_MANAGER_H

#include "DatabaseManager.h"
#include "TransactionDTO.h"
#include <string>
#include <vector>

class TradeManager {
public:
    TradeManager(DatabaseManager& dbManager);
    ~TradeManager();

    void insertTradeHistory(const std::string& tx_type, const std::string& ticker, int amount, double price, const std::string& date);
    void insertCurrentAssets(const std::string& ticker, int amount, double average_price);
    void updateCurrentAssets(const std::string& ticker, int amount, double average_price);
    std::vector<AssetDTO> selectAllFromCurrentAssets();
    std::vector<TransactionDTO> selectAllFromTradeHistory();

private:
    DatabaseManager& dbManager;
};

#endif // TRADE_MANAGER_H
