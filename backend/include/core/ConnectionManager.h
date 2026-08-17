#pragma once

#include <string>
#include <memory>

// Forward declaration of session or connection class
class Connection;

namespace core {

/**
 * @brief Thread-safe registry for tracking active WebSocket clients.
 * 
 * Demonstrates: OS (Concurrency, Shared State, Thread Safety)
 */
class ConnectionManager {
public:
    ConnectionManager();
    ~ConnectionManager();

    /**
     * @brief Adds a new client connection.
     * @param client_id Unique identifier for the client.
     * @param conn Pointer to the connection object.
     */
    void addConnection(const std::string& client_id, std::shared_ptr<Connection> conn);

    /**
     * @brief Removes a client connection safely.
     * @param client_id Unique identifier for the client.
     */
    void removeConnection(const std::string& client_id);

    /**
     * @brief Broadcasts a message to all active clients.
     * @param message The string payload to send.
     */
    void broadcast(const std::string& message);
};

} // namespace core
