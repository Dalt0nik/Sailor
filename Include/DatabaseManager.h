#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <sqlite3.h>

class DatabaseManager {
public:
    DatabaseManager(const std::string& db_name, const std::string& setup_file, const std::string& data_file);
    ~DatabaseManager();

    void execute_sql(const std::string& sql);
    std::string read_sql_file(const std::string& file_path);
    void populate_database();

private:
    sqlite3* db;
    std::string setup_file;
    std::string data_file;

    void initialize_database();
};

#endif // DATABASE_MANAGER_H
