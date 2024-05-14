#include "DatabaseManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& db_name, const std::string& setup_file, const std::string& data_file)
    : setup_file(setup_file), data_file(data_file) {
    int rc = sqlite3_open(db_name.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    } else {
        initialize_database();
    }
}

DatabaseManager::~DatabaseManager() {
    if (db) {
        sqlite3_close(db);
    }
}

void DatabaseManager::execute_sql(const std::string& sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }
}

std::string DatabaseManager::read_sql_file(const std::string& file_path) {
    std::ifstream sql_file(file_path);
    std::stringstream sql_stream;
    if (sql_file.is_open()) {
        sql_stream << sql_file.rdbuf();
        sql_file.close();
    } else {
        std::cerr << "Unable to open file: " << file_path << std::endl;
    }
    return sql_stream.str();
}

void DatabaseManager::initialize_database() {
    std::string sql_setup = read_sql_file(setup_file);
    execute_sql(sql_setup);
}

void DatabaseManager::populate_database() {
    std::string sql_data = read_sql_file(data_file);
    execute_sql(sql_data);
}
