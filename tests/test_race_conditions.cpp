// Test stub for reproducing race conditions
// TODO: Include necessary testing framework headers (e.g., gtest/gtest.h or catch2/catch.hpp)

// TODO: Write a test case that spawns multiple threads attempting to modify a shared resource concurrently without proper locking.
// CS Concept (OS): Concurrency Pitfalls - Race Conditions.
// Verify that the final state is inconsistent, demonstrating the need for synchronization.
#include <iostream>
#include <thread>
#include <vector>
#include "core/LockManager.h"

int shared_counter =0;
core::LockManager lock_manager;
void increment_counter(int thread_id){
    for(int i=0;i<100000;i++){
        lock_manager.acquireLock("counter",thread_id);
        shared_counter++;
        lock_manager.releaseLock("counter",thread_id);
    }
}

int main(int argc, char **argv) {
    std::vector<std::thread> threads;
    for(int i=0;i<10;i++){
        threads.emplace_back(increment_counter,i);
    }
    for(int i=0;i<10;i++){
        threads[i].join();
    }
    std::cout<< shared_counter << std::endl;
    // TODO: Initialize test runner
    return 0;
}
