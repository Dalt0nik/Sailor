#ifndef TRANSACTION_DTO_H
#define TRANSACTION_DTO_H

#include <string>

struct TransactionDTO {
    int id;
    std::string tx_type;
    std::string ticker;
    int amount;
    double price;
    std::string date;
};

struct AssetDTO {
    std::string ticker;
    int amount;
    double average_price;
};

#endif // TRANSACTION_DTO_H