#pragma once

#include <string>

namespace server {

/**
 * @brief Enumeration of protocol message types.
 * 
 * Demonstrates: CN (Application-layer protocol design)
 */
enum class MessageType {
    CHAT,
    LOCK_EVENT,
    DEADLOCK_DETECTED,
    TXN_COMMIT,
    ERROR
};

/**
 * @brief Structure representing a generic protocol payload.
 * 
 * Demonstrates: CN (Data serialization/deserialization structure)
 */
struct ProtocolMessage {
    MessageType type;
    std::string payload;
    long long timestamp;
    
    // No parsing logic implemented here
};

} // namespace server
