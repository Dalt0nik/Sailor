#include <iostream>
#include <fstream>


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
    std::string api_key = readSecrets("secrets.txt");
    std::cout << "Hello, World!"<< std::endl;
    return 0;
}
