#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

namespace core {

/**
 * @brief Lock Manager for coordinating resource access.
 * 
 * Demonstrates: OS (Synchronization, Deadlock Handling)
 * 
 * POSSIBLE DEADLOCK HANDLING STRATEGIES (Choose one to implement):
 * 1. Lock Ordering: Always acquire locks in a globally defined, strict order (e.g., lowest ID first).
 *    Pros: Simple, guarantees no circular wait. Cons: Hard to maintain in complex systems.
 * 2. Wait-Die Scheme: Older transactions wait for younger ones, but younger ones abort if they wait for older ones.
 *    Pros: Non-preemptive, prevents starvation. Cons: Unnecessary rollbacks.
 * 3. Timeout-based Detection: Wait for a lock up to a timeout. If timeout expires, assume deadlock and abort.
 *    Pros: Easy to implement, catches all deadlocks eventually. Cons: Hard to tune timeout, can falsely identify long waits as deadlocks.
 */
class LockManager {
public:
    LockManager();
    ~LockManager();

    /**
     * @brief Attempts to acquire a lock on a resource.
     * @param resource_id The ID of the resource to lock.
     * @param thread_id The ID of the requesting thread.
     * @return true if acquired, false if denied (based on strategy).
     */
    bool acquireLock(const std::string& resource_id, int thread_id);

    /**
     * @brief Releases a previously acquired lock.
     * @param resource_id The ID of the resource to unlock.
     * @param thread_id The ID of the releasing thread.
     */
    void releaseLock(const std::string& resource_id, int thread_id);
private:
    std::unordered_map<std::string, int> resource_owners;
    std::mutex manager_mutex;
    std::condition_variable manager_cv;
    
};

} // namespace core
