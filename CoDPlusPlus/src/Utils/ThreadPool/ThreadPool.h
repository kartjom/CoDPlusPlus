#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <print>

namespace Utils
{
    inline class ThreadPool
    {
    public:
        ThreadPool();
        ~ThreadPool();

        void Initialize(size_t numThreads);
        void Dispose();

        template <class F>
        void Enqueue(F&& task)
        {
            if (!initialized || stop)
            {
                std::println("[Thread Pool] - Enqueue failed due to uninitialized or disposed pool");
                return;
            }

            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(task));
            lock.unlock();
            condition.notify_one();
        };

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queueMutex;
        std::condition_variable condition;
        bool initialized;
        bool stop;
    } ThreadPool;
}