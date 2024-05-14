#include <iostream>
#include <fstream>

#include <cpr/cpr.h>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

#include "DatabaseManager.h"

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

// Function to get the latest available price of a stock
double get_latest_price(const std::string& symbol, const std::string& api_key) {
    std::string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + symbol + "&apikey=" + api_key;
    cpr::Response r = cpr::Get(cpr::Url{ url });

    if (r.status_code != 200) {
        std::cerr << "HTTP error occurred: " << r.status_code << std::endl;
        return -1.0;
    }

    try {
        auto json_response = json::parse(r.text);
        double latest_price = -1.0;

        if (json_response.contains("Global Quote")) {
            latest_price = std::stod(json_response["Global Quote"]["05. price"].get<std::string>());
        }
        else {
            std::cerr << "Error parsing JSON response or no data available." << std::endl;
        }

        return latest_price;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred while parsing JSON: " << e.what() << std::endl;
        return -1.0;
    }
}

int main(int argc, char** argv) {
    std::string api_key = readSecrets("secrets.txt", "api_key");
    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <Stock ticker>" << std::endl;
        return 1;
    }
    
    // Initialize the database
    DatabaseManager dbManager("portfolio.db", "scripts/db-setup.sql", "scripts/mock-data.sql");

    float price = get_latest_price(std::string(argv[1]), api_key);
    std::cout << price;

    return 0;
}
