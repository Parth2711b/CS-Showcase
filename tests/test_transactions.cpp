// Test stub for verifying database transaction atomicity
// TODO: Include necessary testing framework headers (e.g., gtest/gtest.h)

// TODO: Write a test case that initiates a transaction with multiple INSERT statements, but deliberately causes one to fail (e.g., constraint violation).
// CS Concept (DBMS): ACID Properties - Atomicity.
// Verify that the DBConnection successfully rolls back the transaction, and none of the prior successful inserts are committed to the database.

#include "db/DBConnection.h"
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    // TODO: Initialize test runner
    db :: DBConnection my_db;
    my_db.connect("test.db");
    std::vector<std::string> queries;
    queries.push_back("CREATE TABLE IF NOT EXISTS users(id INT PRIMARY KEY, name TEXT);");
    queries.push_back("INSERT INTO users(id,name) VALUES(1,'Charlie');");
    queries.push_back("INSERT INTO users(id,name) VALUES(1,'Phoebus');");
    bool is_success = my_db.executeTransaction(queries);
    if(is_success){
        std::cout<<"Transaction committed successfully"<<std::endl;
    }else{
        std::cout<<"Transaction rolled back"<<std::endl;
    }
    return 0;
}
