#include "db/ConnectionPool.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

// Ye function har ek naya thread (user) run karega
void worker_thread(int id, db::ConnectionPool& pool) {
    std::cout << "Thread " << id << " arrived.\n";
    // TODO: Pool se connection mangiye (pool.getConnection())
    auto conn = pool.getConnection();
    std::cout << "Thread " << id << " found connection\n";
    // TODO: 1 second ke liye thread ko sula dijiye (DBMS ka kaam simulate karne ke liye)
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // TODO: Connection wapas pool mein daal dijiye (pool.releaseConnection())
    pool.releaseConnection(conn);
    std::cout << "Thread " << id << " released connection\n";
}

int main() {
    // TODO: 5 connections ka ek ConnectionPool banaiye (db::ConnectionPool)
    db::ConnectionPool pool(5, "test.db");
    // TODO: Ek std::vector banaiye jo threads ko store kare
    std::vector<std::thread> threads;
    // TODO: Ek loop lagaiye aur 10 threads banaiye. (Har thread ko worker_thread function aur pool ka reference pass kijiye)
    for(int i=0;i<10;i++){
        threads.emplace_back(worker_thread, i, std::ref(pool));
    }
    // TODO: Ek dusra loop lagaiye aur saare threads ko join() kijiye
    for(auto& t : threads){
        t.join();
    }
    std::cout << "All threads finished\n";
    return 0;
}
