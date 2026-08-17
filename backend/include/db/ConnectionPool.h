#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <string>
#include "db/DBConnection.h"

namespace db {

class ConnectionPool {
public:
    /**
     * @brief Initializes the connection pool with a specific number of connections.
     * @param pool_size Number of connections to keep open.
     * @param db_path Path to the SQLite database file.
     */
    ConnectionPool(size_t pool_size, const std::string& db_path);
    ~ConnectionPool();

    /**
     * @brief Gets a connection from the pool. Waits if none are available.
     * @return A shared pointer to a ready-to-use DBConnection.
     */
    std::shared_ptr<DBConnection> getConnection();

    /**
     * @brief Returns a connection back to the pool after use.
     * @param conn The connection to return.
     */
    void releaseConnection(std::shared_ptr<DBConnection> conn);

private:
    std::queue<std::shared_ptr<DBConnection>> available_connections;
    std::mutex pool_mutex;
    std::condition_variable cv;
    // TODO: Yahan ek queue banayiye jo DBConnection pointers ko store kare.
    // TODO: Ek mutex banayiye pool ko crash se bachane ke liye.
    // TODO: Ek condition_variable banayiye threads ko wait karane ke liye jab pool khali ho.
};

} // namespace db
