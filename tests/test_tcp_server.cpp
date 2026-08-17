#include "server/TCPServer.h"
#include <iostream>
#include "server/WebSocketServer.h"

int main() {
    std::cout << "Starting TCP Server...\n"<<std::endl;

    // TODO: 1. Ek port number chuniye (jaise 8080) aur TCPServer ka ek object banaiye
    server::WebSocketServer server1(8080);
    // TODO: 2. server.start() ko call kijiye. Agar fail ho jaye toh "Failed" print karke return 1 kar dijiye.
    if(server1.start()==false){
        std::cout<<"Failed"<<std::endl;
        return 1;
    }
    // TODO: 3. Print kijiye ki "Server is now listening on port X..."
    std::cout<<"Server is now listening on Port 8080"<<std::endl;
    // TODO: 4. server.acceptConnections() ko call kijiye! (Ye infinite chalega aur wait karega)
    server1.acceptConnections();
    return 0;
}
