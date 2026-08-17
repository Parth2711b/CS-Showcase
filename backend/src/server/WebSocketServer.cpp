#include "server/WebSocketServer.h"
#include "server/crypto/sha1.h"
#include "server/crypto/base64.h"
#include <iostream>
#include <vector>
#include <sstream>
namespace server {

WebSocketServer::WebSocketServer(unsigned short port, core::ThreadPool& pool, db::DBConnection& db) 
    : TCPServer(port), threadPool(pool), dbConnection(db) {
}

WebSocketServer::~WebSocketServer() {
}

void WebSocketServer::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);

    char reply_frame[4096];
    reply_frame[0] = 0x81; 
    reply_frame[1] = message.length(); 
    
    for (size_t i = 0; i < message.length(); i++) {
        reply_frame[2 + i] = message[i];
    }

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

        // Demo logic: Check if frontend wants to force a rollback
        bool is_rollback = false;
        if (decoded_msg.find("/rollback ") == 0) {
            is_rollback = true;
            decoded_msg = decoded_msg.substr(10); // Remove the prefix
        }

        std::cout << "\n[Received]: " << decoded_msg << std::endl;
        
        // 1. SQL Query banayein
        std::string sql;
        if (is_rollback) {
            // Intentionally corrupt query to trigger failure
            sql = "INSERT INTO nonexistent_table (content) VALUES ('" + decoded_msg + "');";
        } else {
            sql = "INSERT INTO messages (sender_id, content, timestamp) VALUES (1, '" + decoded_msg + "', 0);";
        }
        
        // 2. Frontend ko batayein ki Transaction shuru ho raha hai
        std::string begin_event = "{\"type\": \"db_event\", \"event\": \"[BEGIN TRANSACTION]\"}";
        broadcastMessage(begin_event);
        
        std::string insert_event = "{\"type\": \"db_event\", \"event\": \"INSERT INTO messages: " + decoded_msg + "\"}";
        broadcastMessage(insert_event);

        // 3. Query run karein (ACID property - Atomicity)
        bool success = dbConnection.executeTransaction({sql});

        // 4. Success ya Failure ka log bhejein
        if (success) {
            std::string commit_event = "{\"type\": \"db_event\", \"event\": \"[COMMIT]\"}";
            broadcastMessage(commit_event);
        } else {
            std::string rollback_event = "{\"type\": \"db_event\", \"event\": \"[ROLLBACK] ERROR\"}";
            broadcastMessage(rollback_event);
        }
        
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

        threadPool.submitTask([this, client_socket]() { this->handleClient(client_socket); });
    }
}

} // namespace server
