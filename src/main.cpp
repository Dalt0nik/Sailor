#include <iostream>
#include <fstream>

#include <cpr/cpr.h>

std::string readSecrets(const std::string fileName){
    std::ifstream secretsFile(fileName);
    std::string api_key;
    if (secretsFile.is_open()) {
        std::getline(secretsFile, api_key); 
        secretsFile.close();
    } else {
        std::cout << "Unable to open secrets file";
    }
    return api_key;
}
int main() {
    std::cout << "Test output" << std::endl;
    std::string api_key = readSecrets("secrets.txt");
    std::cout << "API Key: " << api_key << std::endl;
    cpr::Response r = cpr::Get(cpr::Url{"https://www.alphavantage.co/query?function=CURRENCY_EXCHANGE_RATE&from_currency=BTC&to_currency=CNY&apikey=" + api_key});

    if (r.status_code == 200) { // OK
        std::cout << r.text << std::endl;
    } else {
        std::cout << "HTTP error occurred: " << r.status_code << std::endl;
    }

    std::cout << "Hello, World!" << std::endl;
    system("pause");
    return 0;
}
