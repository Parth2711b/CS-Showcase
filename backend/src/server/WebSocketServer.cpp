#include "server/WebSocketServer.h"
#include "server/crypto/sha1.h"
#include "server/crypto/base64.h"
#include <iostream>
#include <vector>
#include <sstream>
namespace server {

WebSocketServer::WebSocketServer(unsigned short port) : TCPServer(port) {
}

WebSocketServer::~WebSocketServer() {
}

void WebSocketServer::broadcastMessage(const std::string& message) {
    // TODO: Step 1 - Acquire a lock on clients_mutex to prevent race conditions.
    std::lock_guard<std::mutex> lock(clients_mutex);

    // TODO: Step 2 - Create a WebSocket frame for the message (0x81 header + length + payload).
    char reply_frame[4096];
    reply_frame[0] = 0x81; 
    reply_frame[1] = message.length(); 
    
    for (size_t i = 0; i < message.length(); i++) {
        reply_frame[2 + i] = message[i];
    }

    // TODO: Step 3 - Iterate over connected_clients and send the frame to each socket.
    for(SOCKET client : connected_clients){
        send(client ,reply_frame,2+message.length(),0);
    }
}

void WebSocketServer::handleClient(SOCKET client_socket) {
    // This function will run in a separate thread for each connected client.
    
    char buffer[4096] = {0};
    int bytes = recv(client_socket, buffer, 4096, 0);
    if(bytes <= 0) {
        closesocket(client_socket);
        return;
    }

    std::string request(buffer);
    size_t key_pos = request.find("Sec-WebSocket-Key: ");
    if(key_pos == std::string::npos) {
        closesocket(client_socket);
        return;
    }

    // Process WebSocket Handshake
    std::string key = request.substr(key_pos + 19, 24);
    std::string magic = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::vector<uint8_t> hash = crypto::sha1(magic);
    std::string accept_key = crypto::base64_encode(hash);

    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                        "Upgrade: websocket\r\n"
                        "Connection: Upgrade\r\n"
                        "Sec-WebSocket-Accept: " + accept_key + "\r\n\r\n";
    send(client_socket, response.c_str(), response.length(), 0);
    std::cout << "Handshake Complete! Client connected." << std::endl;

    // Get Thread ID for the OS Visualizer
    std::ostringstream thread_id_stream;
    thread_id_stream << std::this_thread::get_id();
    std::string thread_id = thread_id_stream.str();

    // Notify Frontend that a Thread is spawned
    std::string os_event = "{\"type\": \"os_event\", \"event\": \"Thread Spawned\", \"thread_id\": \"" + thread_id + "\"}";
    broadcastMessage(os_event);

    // TODO: Step 4 - Safely add client_socket to connected_clients (requires locking).
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        connected_clients.push_back(client_socket);
    }

    // Read loop for incoming frames
    while (true) {
        char frame[4096] = {0};
        int bytes_received = recv(client_socket, frame, 4096, 0);
        
        if (bytes_received <= 0) {
            std::cout << "Client Disconnected!" << std::endl;
            // TODO: Step 5 - Safely remove client_socket from connected_clients before breaking.
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                for(auto i = connected_clients.begin();i!=connected_clients.end();i++){
                    if(*i==client_socket){
                        connected_clients.erase(i);
                        break;
                    }
                }
            }
            break; 
        }

        // Unmask incoming data
        int payload_length = frame[1] & 0x7F; 
        char masking_key[4];
        for (int i = 0; i < 4; i++) {
            masking_key[i] = frame[2 + i]; 
        }

        std::string decoded_msg = "";
        for (int i = 0; i < payload_length; i++) {
            char decoded_char = frame[6 + i] ^ masking_key[i % 4];
            decoded_msg += decoded_char;
        }

        std::cout << "\n[Received]: " << decoded_msg << std::endl;
        
        // Notify Frontend that Mutex is about to lock
        std::string lock_event = "{\"type\": \"os_event\", \"event\": \"Mutex Locked (Broadcasting...)\", \"thread_id\": \"" + thread_id + "\"}";
        broadcastMessage(lock_event);

        // Broadcast the actual chat message as JSON
        std::string chat_json = "{\"type\": \"chat\", \"sender\": \"Client (T-" + thread_id + ")\", \"text\": \"" + decoded_msg + "\"}";
        broadcastMessage(chat_json);
    }
    
    closesocket(client_socket);
    
    // Notify Frontend that Thread is terminated
    std::string kill_event = "{\"type\": \"os_event\", \"event\": \"Thread Terminated\", \"thread_id\": \"" + thread_id + "\"}";
    broadcastMessage(kill_event);
}

void WebSocketServer::acceptConnections() {
    is_running = true;
    while(is_running){
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if(client_socket == INVALID_SOCKET) continue;
        
        std::cout << "\n--- NEW CONNECTION ACCEPTED ---\n" << std::endl;

        // TODO: Step 7 - Spawn a new std::thread running handleClient(client_socket) and detach it.
        std::thread t(&WebSocketServer::handleClient, this, client_socket);
        t.detach();
    }
}

} // namespace server
