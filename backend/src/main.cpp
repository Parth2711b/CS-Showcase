// Main entry point for the C++ MultiThreaded Server
#include "core/ThreadPool.h"
#include "server/WebSocketServer.h"
#include "db/DBConnection.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SQLite Database connection
    db::DBConnection db;
    db.connect("test.db");

    // Ensure the table exists (Theme according: minimalist schema)
    db.executeTransaction({
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id INTEGER, "
        "content TEXT NOT NULL, "
        "timestamp INTEGER);"
    });
    
    // Initialize ThreadPool with CPU cores
    unsigned int cores = std::thread::hardware_concurrency();
    core::ThreadPool threadPool(cores);
    std::cout << "ThreadPool initialized with " << cores << " worker threads." << std::endl;

    server::WebSocketServer server1(8080, threadPool, db);
    server1.acceptConnections();

    // Main thread typically waits here until a termination signal is received.
    return 0;
}
