#include "server/TCPServer.h"
#include <iostream>

// Needed to link the Winsock library in code (alternative to -lws2_32 in gcc)
#pragma comment(lib, "ws2_32.lib")

namespace server {

TCPServer::TCPServer(int port) {
    this->port = port;
    this->server_socket = INVALID_SOCKET;
    this->is_running = false;
}

TCPServer::~TCPServer() {
    stop();
}

bool TCPServer::start() {
    // TODO: 1. Winsock API ko initialize kijiye (WSAStartup)
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(res!=0)return false;
    // TODO: 2. Ek naya Socket banaiye (socket function) 
    server_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(server_socket==INVALID_SOCKET)return false;
    // TODO: 3. Address setup kijiye (sockaddr_in structure)
    sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port = htons(this->port);
    // TODO: 4. Socket ko is port se bind (jod) dijiye (bind function)
    int binded = bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(binded==SOCKET_ERROR)return false;
    // TODO: 5. Socket ko Listen mode mein daaliye (listen function)
    int listened = listen(server_socket,SOMAXCONN);
    if(listened == SOCKET_ERROR)return false;
    return true;
}

void TCPServer::acceptConnections() {
    is_running=true;
    // TODO: Ek while loop lagaiye jab tak (is_running) true hai
    while(is_running){
        SOCKET client_socket = accept(server_socket,nullptr,nullptr);
        if(client_socket==INVALID_SOCKET)continue;
    // TODO: Loop ke andar accept() function call kijiye naye clients ke liye
    // TODO: Print karvaiye ki "Client Connected!"
        std::cout<<"New Client Connected\n"<<std::endl;
        char buffer[1024] = {0};
        recv(client_socket,buffer,1024,0);
        std::cout<<buffer<<std::endl;
    // TODO: Client ka socket close kar dijiye (closesocket)
        // Browser ke liye ek jawab (HTTP Response) tayar kijiye
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1 style='color:red;'>Hello from C++ Server!</h1></body></html>";
        // Jawab ko Client ko bhej dijiye (send function)
        send(client_socket, response.c_str(), response.length(), 0);
        closesocket(client_socket);
    }
}

void TCPServer::stop() {
    closesocket(server_socket);
    // TODO: server_socket ko close kar dijiye (closesocket)
    WSACleanup();
    // TODO: Winsock ko band kijiye (WSACleanup)
}

} // namespace server
