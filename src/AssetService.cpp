#include "AssetService.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <algorithm> // for std::find_if
#include <stdexcept> // for std::runtime_error
#include <iostream> // for std::cerr

using json = nlohmann::json;

AssetService::AssetService(TradeRepository& tradeRepository, const std::string& api_key)
    : tradeRepository(tradeRepository), api_key(api_key) {}

AssetService::~AssetService() {}

int AssetService::buy_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Insert the trade into trade history
    tradeRepository.insertTradeHistory("BUY", ticker, amount, price, date);

    // Fetch current assets to check if the ticker already exists
    auto currentAssets = tradeRepository.selectAllFromCurrentAssets();
    for (auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            // Update current assets if ticker exists
            int newAmount = asset.amount + amount;
            double newAveragePrice = ((asset.amount * asset.average_price) + (amount * price)) / newAmount;
            tradeRepository.updateCurrentAssets(ticker, newAmount, newAveragePrice);
            return 1;
        }
    }

    // Insert into current assets if ticker does not exist
    tradeRepository.insertCurrentAssets(ticker, amount, price);
    return 1;
}

int AssetService::sell_stock(const std::string& ticker, int amount, double price, const std::string& date) {
    // Fetch current assets to check if the ticker exists
    auto currentAssets = tradeRepository.selectAllFromCurrentAssets();
    for (auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            if (asset.amount < amount) {
                std::cerr << "Not enough shares to sell" << std::endl;
                return -1;
            }
            // Insert the trade into trade history
            tradeRepository.insertTradeHistory("SELL", ticker, amount, price, date);

            int newAmount = asset.amount - amount;
            if (newAmount == 0) {
                // Remove from current assets if all shares are sold
                tradeRepository.updateCurrentAssets(ticker, 0, 0);
            }
            else {
                tradeRepository.updateCurrentAssets(ticker, newAmount, asset.average_price);
            }

            return 1;
        }
    }

    std::cerr << "Ticker not found in current assets" << std::endl;
    return -1;
}

double AssetService::get_total_ticker_value(const std::string& ticker) {
    auto currentAssets = tradeRepository.selectAllFromCurrentAssets();
    for (const auto& asset : currentAssets) {
        if (asset.ticker == ticker) {
            return asset.amount * get_latest_price(asset.ticker);
        }
    }
    std::cerr << "Ticker not found in current assets" << std::endl;
    return -1;
}

double AssetService::get_portfolio_value() {
    auto currentAssets = tradeRepository.selectAllFromCurrentAssets();
    double total_value = 0.0;
    for (const auto& asset : currentAssets) {
        total_value += asset.amount * get_latest_price(asset.ticker);
    }
    return total_value;
}

// Private

// Function to get the latest available price of a stock
double AssetService::get_latest_price(const std::string& symbol) {
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

double AssetService::get_all_expenses_by_ticker(const std::string &ticker){
    return tradeRepository.getAllExpensesByTicker(ticker);
}
double AssetService::get_all_sells_by_ticker(const std::string &ticker){
    return tradeRepository.getAllSellsByTicker(ticker);
}
std::vector<std::string> AssetService::GetAllTickers(){
    return tradeRepository.getAllTickers();
}