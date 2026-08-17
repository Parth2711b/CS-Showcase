#include "db/DBConnection.h"
#include <iostream>
#include <vector>

using namespace std;

namespace db {

DBConnection::DBConnection() {
    // TODO: Initialize connection state here.
}

DBConnection::~DBConnection() {
    close();
    // TODO: Ensure the connection is closed when the object is destroyed.
}

bool DBConnection::connect(const std::string& db_path) {
    int result = sqlite3_open(db_path.c_str(),&db);
    if (result == SQLITE_OK)return true;
    return false;
    // TODO: Implement SQLite connection logic.
    // CS Concept (DBMS): Establish a connection to the database engine.
    // Handle edge case where db_path is invalid or file cannot be opened.
}

bool DBConnection::executeTransaction(const std::vector<std::string>& queries) {
    if(db==nullptr)return false;
    sqlite3_exec(db,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr);
    for(const std::string& query : queries) {
        int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
        if(rc!=SQLITE_OK){
            sqlite3_exec(db,"ROLLBACK;",nullptr,nullptr,nullptr);
            return false;
        }
    }
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
    return true;
    // TODO: Implement transaction execution (BEGIN, loop queries, COMMIT).
    // CS Concept (DBMS): Demonstrate Atomicity - if any query fails, execute ROLLBACK.
    // Edge case: Handle SQL syntax errors or constraint violations during execution.
}

void DBConnection::close() {
    if (db!=nullptr){
        sqlite3_close(db);
        db=nullptr;
    }
    // TODO: Implement SQLite connection closing logic.
}
} // namespace db
