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

void renderMenu(TradeManager &tradeManager){ //also this method should get instance of JpMorganService
    std::string options ="You have several options:\n"
                        "1)BUY a stock\n"
                        "2)SELL stock\n"
                        "3)Check your asset value\n"
                        "4)Check your portfolio value\n"
                        "\'q\' to quit our application\n" ;

    std::cout << "Welcome to the Sailor app! We help to multiply your money $" << std::endl;
    while(1) {
        std::cout << options;
        std::cout << "Type your option" << std::endl;
        char user_option;

        std::string ticker, date;
        int amount;
        double price;
        std::cin >> user_option;

        //it's a mock implementation of the frontend, hence we don't validate user input
        switch(user_option) {
            case '1':
                std::cout << "You choose to BUY a stock. Enter ticker, ammount, price, date (YYYY-mm-dd):" << std::endl;
                std::cin >> ticker >> amount >> price >> date;

                //** CODE GOES HERE //
                break;
            case '2':
                std::cout << "You choose to SELL a stock. Enter your ticker, amount, date(YYYY-mm-dd):" << std::endl;                
                std::cin >> ticker >> amount >> date;
                
                //** CODE GOES HERE //
            
                break;
            case '3':
                std::cout << "You choose to check your asset value. Enter your ticker:" << std::endl;
                std::cin >> ticker;

                //** CODE GOES HERE //
                break;
            case '4':
                std::cout << "You choose to check your portfolio value" << std::endl;

                //** CODE GOES HERE //
                break;                                                

        }

        if(user_option == 'q'){
            std::cout << "Goodbye, old sport." << std::endl;
            break;
        }
    }
}


int main(int argc, char** argv) {
    std::string api_key = readSecrets("secrets.txt", "api_key");
    
    // Initialize the database
    DatabaseManager dbManager("portfolio.db", "scripts/db-setup.sql", "scripts/mock-data.sql");
    
    TradeManager tradeManager(dbManager);
    renderMenu(tradeManager);

    return 0;
}
