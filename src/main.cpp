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
        std::cout << "Unable to open secrets file" << std::endl;
    }
    return api_key;
}
int main(int argc, char** argv) {
    std::string api_key = readSecrets("secrets.txt");
    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <from_currency> <to_currency>" << std::endl;
        return 1;
    }
    std::string url = "https://www.alphavantage.co/query?function=CURRENCY_EXCHANGE_RATE&from_currency=" + std::string(argv[1]) + "&to_currency=" + std::string(argv[2]) + "&apikey=" + api_key;
    cpr::Response r = cpr::Get(cpr::Url{url});

    if (r.status_code == 200) { // OK
        std::cout << r.text << std::endl;
    } else {
        std::cout << "HTTP error occurred: " << r.status_code << std::endl;
    }

    return 0;
}
