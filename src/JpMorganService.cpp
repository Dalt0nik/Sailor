#include "JpMorganService.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <algorithm> // for std::find_if
#include <stdexcept> // for std::runtime_error
#include <iostream> // for std::cerr

using json = nlohmann::json;

JpMorganService::JpMorganService(TradeManager& tradeManager, const std::string& api_key)
    : tradeManager(tradeManager), api_key(api_key) {}

JpMorganService::~JpMorganService() {}

int JpMorganService::buy_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Insert the trade into trade history
    tradeManager.insertTradeHistory("BUY", ticker, amount, price, date);

    // Fetch current assets to check if the ticker already exists
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    for (auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            // Update current assets if ticker exists
            int newAmount = asset.amount + amount;
            double newAveragePrice = ((asset.amount * asset.average_price) + (amount * price)) / newAmount;
            tradeManager.updateCurrentAssets(ticker, newAmount, newAveragePrice);
            return 1;
        }
    }

    // Insert into current assets if ticker does not exist
    tradeManager.insertCurrentAssets(ticker, amount, price);
    return 1;
}

int JpMorganService::sell_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Fetch current assets to check if the ticker exists
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    for (auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            if (asset.amount < amount) {
                std::cerr << "Not enough shares to sell" << std::endl;
                return -1;
            }
            // Insert the trade into trade history
            tradeManager.insertTradeHistory("SELL", ticker, amount, price, date);

            int newAmount = asset.amount - amount;
            if (newAmount == 0) {
                // Remove from current assets if all shares are sold
                tradeManager.updateCurrentAssets(ticker, 0, 0);
            }
            else {
                tradeManager.updateCurrentAssets(ticker, newAmount, asset.average_price);
            }

            return 1;
        }
    }

    std::cerr << "Ticker not found in current assets" << std::endl;
    return -1;
}

double JpMorganService::get_total_ticker_value(const std::string& ticker) {
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    for (const auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            return asset.amount * asset.average_price;
        }
    }
    throw std::runtime_error("Ticker not found in current assets");
}

double JpMorganService::get_portfolio_value() {
    auto currentAssets = tradeManager.selectAllFromCurrentAssets();
    double total_value = 0.0;
    for (const auto& asset : currentAssets) {
        total_value += asset.amount * asset.average_price;
    }
    return total_value;
}

// Private

// Function to get the latest available price of a stock
double JpMorganService::get_latest_price(const std::string& symbol) {
    std::string url = "https://www.alphavantage.co/query?function=GLOBAL_QUOTE&symbol=" + symbol + "&apikey=" + this->api_key;
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
