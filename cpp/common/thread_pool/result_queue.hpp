#pragma once

// std
#include <mutex>
#include <optional>
#include <queue>

namespace tp::exe
{

template <typename T>
class ResultQueue
{
public:
    using Result = T;

    void Push(Result result)
    {
        std::lock_guard lock(queue_mutex_);
        queue_.push(std::move(result));
    }

    bool TryPop(Result& value)
    {
        if (queue_mutex_.try_lock())
        {
            value = queue_.front();
            queue_.pop();
            queue_mutex_.unlock();
            return true;
        }

        return false;
    }

private:
    std::mutex queue_mutex_;
    std::queue<Result> queue_;
};

} // namespace tp::exe
