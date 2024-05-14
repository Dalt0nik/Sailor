#include "JpMorganService.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <algorithm> // for std::find_if
#include <iostream>

using json = nlohmann::json;

JpMorganService::JpMorganService(TradeManager& tradeManager, const std::string& api_key)
    : tradeManager(tradeManager), api_key(api_key) {}

JpMorganService::~JpMorganService() {}

int JpMorganService::buy_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Insert the trade into trade history
    tradeManager.insertTradeHistory("BUY", ticker, amount, price, date);

    // Fetch current assets to check if the ticker already exists
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    auto it = std::find_if(currentAssets.begin(), currentAssets.end(), [&ticker](const AssetDTO& asset) {
        return asset.ticker == ticker;
        });

    if (it != currentAssets.end()) {
        // Update current assets if ticker exists
        int newAmount = it->amount + amount;
        double newAveragePrice = ((it->amount * it->average_price) + (amount * price)) / newAmount;

        tradeManager.updateCurrentAssets(ticker, newAmount, newAveragePrice);
    }
    else {
        // Insert into current assets if ticker does not exist
        tradeManager.insertCurrentAssets(ticker, amount, price);
    }

    return 1;
}

int JpMorganService::sell_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Fetch current assets to check if the ticker exists
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    auto it = std::find_if(currentAssets.begin(), currentAssets.end(), [&ticker](const AssetDTO& asset) {
        return asset.ticker == ticker;
        });

    if (it != currentAssets.end()) {
        if (it->amount < amount) {
            std::cout << "Not enough shares to sell" << std::endl;
            return -1;
        }
        else {
            // Insert the trade into trade history
            tradeManager.insertTradeHistory("SELL", ticker, amount, price, date);

            int newAmount = it->amount - amount;

            if (newAmount == 0) {
                // Remove from current assets if all shares are sold
                tradeManager.updateCurrentAssets(ticker, 0, 0);
            }
            else {
                tradeManager.updateCurrentAssets(ticker, newAmount, it->average_price);
            }

            return 1;
        }
    }
    else {
        std::cout << "Ticker not found in current assets" << std::endl;
        return -1;
    }
}

double JpMorganService::get_total_ticker_value(const std::string& ticker) {
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    auto it = std::find_if(currentAssets.begin(), currentAssets.end(), [&ticker](const AssetDTO& asset) {
        return asset.ticker == ticker;
        });

    if (it != currentAssets.end()) {
        return it->amount * get_latest_price(ticker);
    }
    else {
        std::cout << "Ticker not found in current assets";
    }
}

double JpMorganService::get_portfolio_value() {
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    double total_value = 0.0;

    for (const auto& asset : currentAssets) {
        total_value += get_total_ticker_value(asset.ticker);
    }

    return total_value;
}

// Private

// Function to get the latest available price of a stock
double JpMorganService::get_latest_price(const std::string& symbol) {
    std::string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + symbol + "&apikey=" + this->api_key;
    cpr::Response r = cpr::Get(cpr::Url{ url });

    if (r.status_code != 200) {
        std::cout << "HTTP error occurred: " << r.status_code << std::endl;
        return -1.0;
    }

    try {
        auto json_response = json::parse(r.text);
        double latest_price = -1.0;

        if (json_response.contains("Global Quote")) {
            latest_price = std::stod(json_response["Global Quote"]["05. price"].get<std::string>());
        }
        else {
            std::cout << "Error parsing JSON response or no data available." << std::endl;
        }

        return latest_price;
    }
    catch (const std::exception& e) {
        std::cout << "Exception occurred while parsing JSON: " << e.what() << std::endl;
        return -1.0;
    }
}
