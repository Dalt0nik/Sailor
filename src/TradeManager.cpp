#include "TradeManager.h"
#include <iostream>

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

std::vector<std::map<std::string, std::string>> TradeManager::selectAllFromCurrentAssets() {
    std::string sql = "SELECT * FROM current_assets";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    std::vector<std::map<std::string, std::string>> results;

    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::map<std::string, std::string> row;
            int column_count = sqlite3_column_count(stmt);
            for (int i = 0; i < column_count; i++) {
                row[sqlite3_column_name(stmt, i)] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            }
            results.push_back(row);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}

std::vector<std::map<std::string, std::string>> TradeManager::selectAllFromTradeHistory() {
    std::string sql = "SELECT * FROM trade_history";
    sqlite3_stmt* stmt = dbManager.prepare_statement(sql);
    std::vector<std::map<std::string, std::string>> results;

    if (stmt) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::map<std::string, std::string> row;
            int column_count = sqlite3_column_count(stmt);
            for (int i = 0; i < column_count; i++) {
                row[sqlite3_column_name(stmt, i)] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            }
            results.push_back(row);
        }
        sqlite3_finalize(stmt);
    }
    return results;
}
