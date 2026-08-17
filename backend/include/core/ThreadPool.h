#pragma once
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace core {
/**
 * @brief A standard thread pool for background task execution.
 * 
 * Demonstrates: OS (Thread Management, Task Scheduling, Concurrency)
 */
class ThreadPool {
public:
    /**
     * @brief Initializes the thread pool with a specific number of workers.
     * @param num_threads The number of worker threads to spawn.
     */
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    /**
     * @brief Submits a new task to be executed by a worker thread.
     * @param task A callable function representing the task.
     */
    void submitTask(std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
    /**
     * @brief The loop executed by each worker thread, waiting for and processing tasks.
     */
    void workerLoop();
};

} // namespace core
