#include "server/TCPServer.h"
#include <iostream>
#include "server/WebSocketServer.h"
#include "core/ThreadPool.h"
#include "db/DBConnection.h"
int main() {
    std::cout << "Starting TCP Server...\n"<<std::endl;

    db::DBConnection db;
    db.connect("test.db");

    // Ensure the table exists
    db.executeTransaction({
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "sender_id INTEGER, "
        "content TEXT NOT NULL, "
        "timestamp INTEGER);"
    });

    core::ThreadPool pool(4);
    server::WebSocketServer server1(8080, pool, db);
    if(server1.start()==false){
        std::cout<<"Failed"<<std::endl;
        return 1;
    }
    std::cout<<"Server is now listening on Port 8080"<<std::endl;
    server1.acceptConnections();
    return 0;
}
