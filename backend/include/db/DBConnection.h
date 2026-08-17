#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

namespace db {

/**
 * @brief SQLite Database Connection Manager
 * 
 * Demonstrates: DBMS (Connections, Transactions, ACID properties)
 */
class DBConnection {
public:
    DBConnection();
    ~DBConnection();

    /**
     * @brief Connects to the SQLite database.
     * @param db_path Path to the SQLite file.
     * @return true if successful, false otherwise.
     */
    bool connect(const std::string& db_path);

    /**
     * @brief Executes a transaction with multiple SQL statements.
     * 
     * Demonstrates: DBMS Atomicity - ensures all operations succeed or all fail.
     * @param queries List of SQL queries to execute together.
     * @return true if transaction committed successfully, false if rolled back.
     */
    bool executeTransaction(const std::vector<std::string>& queries);

    /**
     * @brief Closes the database connection safely.
     */
    void close();

private: 
    sqlite3* db=nullptr;
};

} // namespace db
