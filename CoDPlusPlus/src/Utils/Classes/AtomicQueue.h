#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace Utils
{
    template <typename T>
    class AtomicQueue
    {
    public:
        void push(const T& value);
        bool pop(T& value);
        bool wait_for_value();

    private:
        std::queue<T> queue_;
        std::mutex mutex_;
        std::condition_variable condition_;
    };
}

namespace Utils /* Implementation */
{
    template <typename T>
    void AtomicQueue<T>::push(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        condition_.notify_one();
    }

    template <typename T>
    bool AtomicQueue<T>::pop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!queue_.empty())
        {
            value = queue_.front();
            queue_.pop();

            return true;
        }

        return false;
    }

    template <typename T>
    bool AtomicQueue<T>::wait_for_value()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty(); });

        return !queue_.empty();
    }
}