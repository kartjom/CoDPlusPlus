#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#include <thread>
#include <functional>
#include <print>
#include <thread>
#include <memory>

namespace Utils
{
    struct TaskContext
    {
        std::function<void()> task;
    };

    inline class ThreadPool
    {
    public:
        ThreadPool();
        ~ThreadPool();

        void Initialize();
        void Dispose();

        template <class F>
        bool Enqueue(F&& task)
        {
            if (pool == NULL)
            {
                std::println("[Threading] - Enqueue failed due to uninitialized or disposed pool");
                return false;
            }

            TaskContext* taskContext = new TaskContext();
            taskContext->task = std::forward<F>(task);

            return this->EnqueueInternal(taskContext);
        };

        bool EnqueueInternal(TaskContext* taskContext);

    private:
        PTP_POOL pool = NULL;
        TP_CALLBACK_ENVIRON callbackEnv;
        PTP_CLEANUP_GROUP cleanupGroup;  

    } ThreadPool;
}