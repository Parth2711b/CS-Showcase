#include "db/DBConnection.h"
#include <iostream>
#include <vector>

using namespace std;

namespace db {

DBConnection::DBConnection() {
}

DBConnection::~DBConnection() {
    close();
}

bool DBConnection::connect(const std::string& db_path) {
    int result = sqlite3_open(db_path.c_str(),&db);
    if (result == SQLITE_OK)return true;
    return false;
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
}

void DBConnection::close() {
    if (db!=nullptr){
        sqlite3_close(db);
        db=nullptr;
    }
}
} // namespace db
