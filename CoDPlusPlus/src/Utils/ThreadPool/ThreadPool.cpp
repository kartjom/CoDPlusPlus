#include "ThreadPool.h"
#include <print>

namespace Utils
{
    ThreadPool::ThreadPool()
        : initialized(false), stop(false)
    {
    }
    
    ThreadPool::~ThreadPool()
    {
        Dispose();
    }

    void ThreadPool::Initialize(size_t numThreads)
    {
        initialized = true;

        int created = 0;
        for (size_t i = 0; i < numThreads; i++)
        {
            try
            {
                workers.emplace_back([this] {
                    for (;;)
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });

                        if (stop && tasks.empty())
                            return;

                        if (tasks.empty()) continue;

                        auto task = std::move(tasks.front());
                        tasks.pop();
                        lock.unlock();

                        try
                        {
                            task();
                        }
                        catch (std::exception& e)
                        {
                            std::println("[Thread Pool] - exception when executing task: {}", e.what());
                        }
                    }
                });

                created++;
            }
            catch (std::exception& e)
            {
                std::println("[Thread Pool] - exception when creating thread {}: {}", i, e.what());
            }
        }

        std::println("[Thread Pool] - Initialized {} threads", created);
    }

    void ThreadPool::Dispose()
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;

        lock.unlock();
        condition.notify_all();

        for (std::thread& worker : workers)
        {
            worker.join();
        }
    }
}