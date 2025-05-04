#include "ThreadPool.h"

namespace Threading
{
    ThreadPool::ThreadPool()
    {
    }
    
    ThreadPool::~ThreadPool()
    {
        Dispose();
    }

    void ThreadPool::Initialize()
    {
        pool = CreateThreadpool(NULL);
        if (pool == NULL)
        {
            std::println("[Threading] - CreateThreadpool failed. Error: {}", GetLastError());
            return;
        }

        int threadCount = std::thread::hardware_concurrency();
        SetThreadpoolThreadMinimum(pool, threadCount);
        SetThreadpoolThreadMaximum(pool, threadCount * 2);

        // Initialize the callback environment
        InitializeThreadpoolEnvironment(&callbackEnv);

        // Create a cleanup group for managing resources
        cleanupGroup = CreateThreadpoolCleanupGroup();
        if (cleanupGroup == NULL)
        {
            std::println("[Threading] - CreateThreadpoolCleanupGroup failed. Error: {}", GetLastError());
            CloseThreadpool(pool);
            pool = NULL;

            return;
        }

        // Associate the thread pool with the cleanup group
        SetThreadpoolCallbackPool(&callbackEnv, pool);
        SetThreadpoolCallbackCleanupGroup(&callbackEnv, cleanupGroup, NULL);

        std::println("[Threading] - Thread Pool Initialized");
    }

    void ThreadPool::Dispose()
    {
        if (pool == NULL) return;

        CloseThreadpoolCleanupGroupMembers(cleanupGroup, FALSE, NULL);
        CloseThreadpoolCleanupGroup(cleanupGroup);
        DestroyThreadpoolEnvironment(&callbackEnv);
        CloseThreadpool(pool);

        pool = NULL;

        std::println("[Threading] - Thread Pool Disposed");
    }

    void ThreadPool::Clear()
    {
        if (pool == NULL) return;

        CloseThreadpoolCleanupGroupMembers(cleanupGroup, TRUE, nullptr);
    }

    void CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work)
    {
        TaskContext* taskContext = (TaskContext*)parameter;

        try
        {
            taskContext->task();
        }
        catch (std::exception& e)
        {
            std::println("[Threading] - exception when executing task: Error: {}", e.what());
        }
        
        delete taskContext;
        CloseThreadpoolWork(work);
    }

    bool ThreadPool::EnqueueInternal(TaskContext* taskContext)
    {
        PTP_WORK work = CreateThreadpoolWork(WorkCallback, taskContext, &callbackEnv);
        if (work == NULL)
        {
            std::println("[Threading] - CreateThreadpoolWork failed. Error: {}", GetLastError());
            delete taskContext;

            return false;
        }

        SubmitThreadpoolWork(work);

        return true;
    }
}