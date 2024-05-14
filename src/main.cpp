#include <iostream>
#include <fstream>

#include <cpr/cpr.h>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

#include "DatabaseManager.h"
#include "TradeManager.h"
#include <JpMorganService.h>

using json = nlohmann::json;

// Function to read API key from a secret
std::string readSecrets(const std::string& fileName, const std::string& keyToFind) {
    std::ifstream secretsFile(fileName);
    std::string line;
    std::string value = ""; // Initialize to empty string in case key is not found

    if (secretsFile.is_open()) {
        while (std::getline(secretsFile, line)) {
            // Find the position of '='
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                // Extract the key and value from the line
                std::string key = line.substr(0, equalsPos);
                value = line.substr(equalsPos + 1);

                // Remove leading and trailing whitespaces from key and value
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                // Check if the key matches the keyToFind
                if (key == keyToFind) {
                    break; // Key found, no need to continue searching
                }
            }
        }
        secretsFile.close();
    } else {
        std::cout << "Unable to open secrets file" << std::endl;
    }
    return value;
}

int main(int argc, char** argv) {
    std::string api_key = readSecrets("secrets.txt", "api_key");
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <Stock ticker>" << std::endl;
        return 1;
    }
    
    // Initialize the database
    DatabaseManager dbManager("portfolio.db", "scripts/db-setup.sql", "scripts/mock-data.sql");
    
    TradeManager tradeManager(dbManager);
    
    JpMorganService jpMorganService(tradeManager, api_key);

    std::cout << jpMorganService.buy_stock("MSFT", 50, 125, "2024-01-02");
    std::cout << jpMorganService.sell_stock("MSFT", 25, 150, "2024-01-05");
    std::cout << jpMorganService.get_total_ticker_value("HEGE") << std::endl;
    std::cout << jpMorganService.get_portfolio_value() << std::endl;

    return 0;
}
