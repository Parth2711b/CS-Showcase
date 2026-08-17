#include "db/ConnectionPool.h"

namespace db {

ConnectionPool::ConnectionPool(size_t pool_size, const std::string& db_path) {
    // TODO: 'pool_size' baar loop chalayiye.
    for(int i=0;i<pool_size;i++){
        auto conn = std::make_shared<DBConnection>(); 
        conn->connect(db_path); 
        available_connections.push(conn);
    }
    // Naya DBConnection banaiye, usko connect() kijiye, aur queue mein daal dijiye.
}

ConnectionPool::~ConnectionPool() {
    while(!available_connections.empty()){
        auto conn = available_connections.front();
        available_connections.pop(); 
        conn->close();
    }
    // TODO: Queue khali kijiye aur saare connections ko close() kar dijiye.
}

std::shared_ptr<DBConnection> ConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(pool_mutex);
    while(available_connections.empty()){
        cv.wait(lock);
    }
    auto conn = available_connections.front();
    available_connections.pop(); 
    return conn;
    // TODO: Lock lagaiye, agar queue khali hai toh wait kijiye. 
    // Jab connection mil jaye, queue se nikaliye aur return kar dijiye.
}

void ConnectionPool::releaseConnection(std::shared_ptr<DBConnection> conn) {
    std::unique_lock<std::mutex> lock(pool_mutex);
    available_connections.push(conn);
    cv.notify_all();
    // TODO: Lock lagaiye, connection ko wapas queue mein daaliye, aur sote hue threads ko jagaiye.
}

} // namespace db
