#pragma once

#include <string>

namespace models {

/**
 * @brief User data structure
 * 
 * Demonstrates: OOP (Encapsulation, Data Modeling)
 */
struct User {
    int id;
    std::string username;
    
    // Constructors only, no logic
    User() = default;
    User(int id, const std::string& username);
};

/**
 * @brief Message/Task data structure
 * 
 * Demonstrates: OOP (Encapsulation)
 */
struct Message {
    int id;
    int sender_id;
    std::string content;
    long long timestamp;

    Message() = default;
    Message(int id, int sender, const std::string& msg, long long ts);
};

/**
 * @brief Session data structure for an active user
 * 
 * Demonstrates: OOP (State Management)
 */
struct Session {
    std::string session_token;
    int user_id;
    long long created_at;

    Session() = default;
    Session(const std::string& token, int uid, long long time);
};

} // namespace models
