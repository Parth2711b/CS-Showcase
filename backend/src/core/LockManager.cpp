#include "core/LockManager.h"

namespace core {

LockManager::LockManager() {
    // TODO: Initialize internal lock tracking structures.
}

LockManager::~LockManager() {
    // TODO: Ensure all locks are properly released on destruction.
}

bool LockManager::acquireLock(const std::string& resource_id, int thread_id) {
    std::unique_lock<std::mutex> lock(manager_mutex);
    manager_cv.wait(lock, [this,resource_id]{return resource_owners.find(resource_id)==resource_owners.end();});
    resource_owners.insert({resource_id,thread_id});
    // TODO: Implement lock acquisition based on the chosen strategy (Wait-Die, Lock Ordering, etc.).
    // CS Concept (OS): Demonstrate synchronization and deadlock prevention/detection.
    // Edge case: A thread attempts to lock a resource it already holds.
    // Edge case: Resource is held by another thread, potentially causing a circular wait.
    return true;
}

void LockManager::releaseLock(const std::string& resource_id, int thread_id) {
    std::unique_lock<std::mutex> lock(manager_mutex);
    if(resource_owners[resource_id]==thread_id){
        resource_owners.erase(resource_id);
    }
    manager_cv.notify_all();
    // TODO: Implement lock release and notify any waiting threads.
    // CS Concept (OS): Demonstrate proper resource deallocation.
    // Edge case: A thread attempts to release a lock it does not hold.
}

} // namespace core
