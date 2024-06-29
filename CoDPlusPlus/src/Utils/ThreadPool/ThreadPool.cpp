#include "ThreadPool.h"

namespace Utils
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
            std::println("[Thread Pool] - CreateThreadpool failed. Error: {}", GetLastError());
            return;
        }

        SetThreadpoolThreadMinimum(pool, std::thread::hardware_concurrency());
        SetThreadpoolThreadMaximum(pool, 64);

        // Initialize the callback environment
        InitializeThreadpoolEnvironment(&callbackEnv);

        // Create a cleanup group for managing resources
        cleanupGroup = CreateThreadpoolCleanupGroup();
        if (cleanupGroup == NULL)
        {
            std::println("[Thread Pool] - CreateThreadpoolCleanupGroup failed. Error: {}", GetLastError());
            CloseThreadpool(pool);
            pool = NULL;

            return;
        }

        // Associate the thread pool with the cleanup group
        SetThreadpoolCallbackPool(&callbackEnv, pool);
        SetThreadpoolCallbackCleanupGroup(&callbackEnv, cleanupGroup, NULL);

        std::println("[Thread Pool] - Initialized");
    }

    void ThreadPool::Dispose()
    {
        if (pool == NULL) return;

        CloseThreadpoolCleanupGroupMembers(cleanupGroup, FALSE, NULL);
        CloseThreadpoolCleanupGroup(cleanupGroup);
        DestroyThreadpoolEnvironment(&callbackEnv);
        CloseThreadpool(pool);

        pool = NULL;

        std::println("[Thread Pool] - Disposed");
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
            std::println("[Thread Pool] - exception when executing task: Error: {}", e.what());
        }
        
        delete taskContext;
        CloseThreadpoolWork(work);
    }

    bool ThreadPool::EnqueueInternal(TaskContext* taskContext)
    {
        PTP_WORK work = CreateThreadpoolWork(WorkCallback, taskContext, &callbackEnv);
        if (work == NULL)
        {
            std::println("[Thread Pool] - CreateThreadpoolWork failed. Error: {}", GetLastError());
            delete taskContext;

            return false;
        }

        SubmitThreadpoolWork(work);

        return true;
    }
}