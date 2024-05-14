#include "TradeManager.h"
#include <iostream>
#include <sstream>

TradeManager::TradeManager(DatabaseManager& dbManager) : dbManager(dbManager) {}

TradeManager::~TradeManager() {}

void TradeManager::insertTradeHistory(const std::string& tx_type, const std::string& ticker, int amount, double price, const std::string& date) {
    std::string sql = "INSERT INTO trade_history (tx_type, ticker, amount, price, date) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    if (stmt) {
        sqlite3_bind_text(stmt, 1, tx_type.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, ticker.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, amount);
        sqlite3_bind_double(stmt, 4, price);
        sqlite3_bind_text(stmt, 5, date.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(dbManager.get_db()) << std::endl;
        }

        sqlite3_finalize(stmt);
    }
}
void TradeManager::insertCurrentAssets(const std::string& ticker, int amount, double average_price) {
    std::string sql = "INSERT INTO current_assets (ticker, amount, average_price) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    if (stmt) {
        sqlite3_bind_text(stmt, 1, ticker.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, amount);
        sqlite3_bind_double(stmt, 3, average_price);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(dbManager.get_db()) << std::endl;
        }

        sqlite3_finalize(stmt);
    }
}

void TradeManager::updateCurrentAssets(const std::string& ticker, int amount, double average_price) {
    std::string sql = "UPDATE current_assets SET amount = ?, average_price = ? WHERE ticker = ?";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    if (stmt) {
        sqlite3_bind_int(stmt, 1, amount);
        sqlite3_bind_double(stmt, 2, average_price);
        sqlite3_bind_text(stmt, 3, ticker.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(dbManager.get_db()) << std::endl;
        }

        sqlite3_finalize(stmt);
    }
}

std::vector<AssetDTO> TradeManager::selectAllFromCurrentAssets() {
    std::string sql = "SELECT * FROM current_assets";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    std::vector<AssetDTO> results;

    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AssetDTO asset;
            asset.ticker = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            asset.amount = sqlite3_column_int(stmt, 1);
            asset.average_price = sqlite3_column_double(stmt, 2);
            results.push_back(asset);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}

std::vector<TransactionDTO> TradeManager::selectAllFromTradeHistory() {
    std::string sql = "SELECT * FROM trade_history";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    std::vector<TransactionDTO> results;

    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            TransactionDTO transaction;
            transaction.id = sqlite3_column_int(stmt, 0);
            transaction.tx_type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            transaction.ticker = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            transaction.amount = sqlite3_column_int(stmt, 3);
            transaction.price = sqlite3_column_double(stmt, 4);
            transaction.date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            results.push_back(transaction);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}
