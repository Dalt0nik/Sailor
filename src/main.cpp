#include <iostream>
#include <fstream>

#include <cpr/cpr.h>
#include <sqlite3.h>

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

// Execute SQL statements
void execute_sql(sqlite3* db, const std::string& sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }
}

// Read SQL from a file
std::string read_sql_file(const std::string& file_path) {
    std::ifstream sql_file(file_path);
    std::stringstream sql_stream;
    if (sql_file.is_open()) {
        sql_stream << sql_file.rdbuf();
        sql_file.close();
    }
    else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
    return sql_stream.str();
}


// Initialize the sqlite database in the root
sqlite3* initialize_database(const std::string& db_name, const std::string& setup_file) {
    sqlite3* db;
    int rc = sqlite3_open(db_name.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    // Read and execute the setup SQL file
    std::string sql_setup = read_sql_file(setup_file);
    execute_sql(db, sql_setup);

    return db;
}

// Function to populate the database with mock data
void populate_database(sqlite3* db, const std::string& data_file) {
    std::string sql_data = read_sql_file(data_file);
    execute_sql(db, sql_data);
}

int main(int argc, char** argv) {
    std::string api_key = readSecrets("secrets.txt", "api_key");
    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <from_currency> <to_currency>" << std::endl;
        return 1;
    }
    
    // Initialize the database
    sqlite3* db = initialize_database("portfolio.db", "scripts/db-setup.sql");
    if (!db) {
        return 1;
    }

    // Populate the database with mock data
    populate_database(db, "scripts/mock-data.sql");

    // Send request to Alpha Vantage
    std::string url = "https://www.alphavantage.co/query?function=CURRENCY_EXCHANGE_RATE&from_currency=" + std::string(argv[1]) + "&to_currency=" + std::string(argv[2]) + "&apikey=" + api_key;
    cpr::Response r = cpr::Get(cpr::Url{url});

    // Print current rate
    if (r.status_code == 200) { // OK
        std::cout << r.text << std::endl;
    } else {
        std::cout << "HTTP error occurred: " << r.status_code << std::endl;
    }

    return 0;
}
