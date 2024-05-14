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

#endif // TRANSACTION_DTO_H