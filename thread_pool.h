// =============================================================================
// HopStar Server - Generic Thread Pool
// Thread pool for parallel task execution
// =============================================================================
#pragma once

#include "server_common.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <future>
#include <vector>

namespace HopStarServer {

// ============================================================================
// Generic Thread Pool
// ============================================================================
class ThreadPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    mutable std::mutex queue_mutex;
    std::condition_variable cv;
    bool stop = false;

    size_t thread_count;
    std::string pool_name;

    void workerLoop() {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this] { return stop || !tasks.empty(); });

                if (stop && tasks.empty()) {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            try {
                task();
            } catch (const std::exception& e) {
                LOG_ERROR(pool_name + " task exception: " + std::string(e.what()));
            } catch (...) {
                LOG_ERROR(pool_name + " task exception: unknown error");
            }
        }
    }

public:
    ThreadPool(size_t num_threads, const std::string& name = "ThreadPool")
        : thread_count(num_threads), pool_name(name) {}

    ~ThreadPool() {
        shutdown();
    }

    // Start the thread pool
    void start() {
        if (!workers.empty()) {
            LOG_WARN(pool_name + " already started");
            return;
        }

        LOG_INFO(pool_name + " starting with " + std::to_string(thread_count) + " threads");

        for (size_t i = 0; i < thread_count; ++i) {
            workers.emplace_back(&ThreadPool::workerLoop, this);
        }
    }

    // Submit a task and get a future for the result
    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();

        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            if (stop) {
                throw std::runtime_error(pool_name + " is stopped, cannot submit task");
            }

            tasks.emplace([task]() { (*task)(); });
        }

        cv.notify_one();
        return result;
    }

    // Submit a task without getting a future (fire and forget)
    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            if (stop) {
                LOG_WARN(pool_name + " is stopped, cannot enqueue task");
                return;
            }

            tasks.emplace(std::move(task));
        }

        cv.notify_one();
    }

    // Shutdown the thread pool
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (stop) return;  // Already stopped
            stop = true;
        }

        cv.notify_all();

        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        workers.clear();
        LOG_INFO(pool_name + " shutdown complete");
    }

    // Get current queue size
    size_t queueSize() const {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return tasks.size();
    }

    // Get thread count
    size_t threadCount() const {
        return thread_count;
    }

    // Get pool name
    std::string name() const {
        return pool_name;
    }

    // Check if pool is stopped
    bool isStopped() const {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return stop;
    }
};

} // namespace HopStarServer
