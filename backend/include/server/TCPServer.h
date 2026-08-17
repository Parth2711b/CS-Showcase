#pragma once
#include <string>
// Windows Sockets API headers
#include <winsock2.h>
#include <ws2tcpip.h>

namespace server {

class TCPServer {
public:
    TCPServer(int port);
    virtual ~TCPServer();

    // Starts the server and begins listening on the port
    bool start();

    // Infinite loop to accept clients
    virtual void acceptConnections();

    // Safely stops the server
    void stop();

protected:
    int port;
    SOCKET server_socket; // 'SOCKET' is a special type provided by winsock2.h
    bool is_running;
};

} // namespace server
