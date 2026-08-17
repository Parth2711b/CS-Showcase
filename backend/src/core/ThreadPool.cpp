#include "core/ThreadPool.h"

namespace core {

ThreadPool::ThreadPool(size_t num_threads) {
    for(int i=0;i<num_threads;i++){
        workers.push_back(std::thread([this]{this->workerLoop();}));
    }
    // TODO: Spawn 'num_threads' worker threads and start the worker loop for each.
    // CS Concept (OS): Thread creation and lifecycle management.
}

ThreadPool::~ThreadPool() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
    condition.notify_all();
    for(std::thread &worker : workers){
        if(worker.joinable()){
            worker.join();
        }
    }
    // TODO: Signal all threads to terminate and join them gracefully.
    // CS Concept (OS): Graceful thread shutdown and resource cleanup.
}

void ThreadPool::submitTask(std::function<void()> task) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    tasks.push(task);
    condition.notify_one();
    // TODO: Add the task to the queue and notify a waiting worker thread via condition_variable.
    // CS Concept (OS): Producer-consumer problem (Producer side).
}

void ThreadPool::workerLoop() {
    while(true){
        std::function<void()> task;
        std::unique_lock<std::mutex> lock(queue_mutex);
        condition.wait(lock, [this]{return !tasks.empty() || stop;});
        if(stop && tasks.empty()){
            return;
        }
        task = tasks.front();
        tasks.pop();
        lock.unlock();
        task();
    }
    // TODO: Wait on a condition_variable for tasks, retrieve them from the queue, and execute.
    // CS Concept (OS): Producer-consumer problem (Consumer side).
    // Edge case: Spurious wakeups from the condition variable.
}

} // namespace core
