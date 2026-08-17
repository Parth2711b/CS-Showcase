#pragma once
#include "TCPServer.h"
#include <string>
#include <vector>
#include <mutex>
#include <thread>


namespace server {

/**
 * @brief Boost.Beast-based WebSocket Server
 * 
 * Demonstrates: CN (Server-side Sockets, Application Protocols) & OS (Multi-threading)
 */
class WebSocketServer : public TCPServer {
public:
    /**
     * @brief Initializes the server to listen on a specific port.
     * @param port The port to bind to.
     */
    explicit WebSocketServer(unsigned short port);
    ~WebSocketServer();
    /**
     * @brief Accepts a new incoming connection and spawns a per-client session/thread.
     */
    void acceptConnections()override;
private:
    // OS Concept: Mutex (Tala) race condition rokne ke liye
    std::mutex clients_mutex;
    
    // List: Jisme hum saare connected logon ko yaad rakhenge
    std::vector<SOCKET> connected_clients;

    // Worker Function: Ye har naye client ke liye alag Thread me chalega
    void handleClient(SOCKET client_socket);

    // Ye ek ka message sabko bhejega (Broadcast)
    void broadcastMessage(const std::string& message);

};

} // namespace server
