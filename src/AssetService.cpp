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

    std::cout << r.text;

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
            std::cerr << "Error parsing JSON response or no data available for the latest price." << std::endl;
        }

        return latest_price;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred while parsing JSON: " << e.what() << std::endl;
        return -1.0;
    }
}

double AssetService::get_all_buys_from_date(const std::string &ticker, const std::string &date_from){
    return tradeRepository.getAllBuysFromDate(ticker, date_from);
}
double AssetService::get_all_sells_from_date(const std::string &ticker, const std::string &date_from){
    return tradeRepository.getAllSellsFromDate(ticker, date_from);
}
std::vector<std::string> AssetService::GetAllTickers(){
    return tradeRepository.getAllTickers();
}
double AssetService::get_existing_assets_amount_by_date(const std::string &ticker, const std::string &date_from){
    return tradeRepository.getExistingAssetsValueByDate(ticker, date_from);
}

double AssetService::get_price_by_date(const std::string &ticker, const std::string &date) {
    std::string url = "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=" + ticker + "&outputsize=full&interval=60min&apikey=" + this->api_key + "&month=" + date.substr(0, 7);
    cpr::Response r = cpr::Get(cpr::Url{ url });

    std::cout << "url: " << url << std::endl;

    std::cout << "r: " << r.text << std::endl;

    if (r.status_code != 200) {
        std::cerr << "HTTP error occurred: " << r.status_code << std::endl;
        return -1.0;
    }



    try {
        auto json_response = json::parse(r.text);
        std::string latest_timestamp;
        double closing_price = -1.0;

        if (json_response.contains("Time Series (60min)")) {
            auto time_series = json_response["Time Series (60min)"];

            for (auto it = time_series.begin(); it != time_series.end(); ++it) {
                std::string timestamp = it.key();
                if (timestamp.substr(0, 10) == date) {
                    if (latest_timestamp.empty() || timestamp > latest_timestamp) {
                        latest_timestamp = timestamp;
                        closing_price = std::stod(it.value()["4. close"].get<std::string>());
                    }
                }
            }

            if (!latest_timestamp.empty()) {
                return closing_price;
            } else {
                std::cerr << "No data available for the specified date: " << date << std::endl;
                return -1.0;
            }
        } else {
            std::cerr << "Error parsing JSON response or no data available in the specific date." << std::endl;
            return -1.0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred while parsing JSON: " << e.what() << std::endl;
        return -1.0;
    }
}

double AssetService::calculate_ticker_profit(const std::string &ticker, const std::string &date_from) {
    double existing_assets_amount = get_existing_assets_amount_by_date(ticker, date_from); //another expense
    double existing_assets_value = existing_assets_amount * get_price_by_date(ticker, date_from);
    
    
    double total_ticker_value = get_total_ticker_value(ticker);
    if (total_ticker_value == -1.0) {
        total_ticker_value = 0;
    }

    double sells = get_all_sells_from_date(ticker, date_from);
    double buys = get_all_buys_from_date(ticker, date_from);

    if(sells == -1|| buys == -1){
        return -1;
    }

    return total_ticker_value - existing_assets_value + sells - buys;
}
