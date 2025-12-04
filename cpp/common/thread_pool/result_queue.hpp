#pragma once

// std
#include <mutex>
#include <optional>
#include <queue>
#include <vector>

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
            if (!queue_.empty())
            {
                value = queue_.front();
                queue_.pop();
                queue_mutex_.unlock();
                return true;
            }

            queue_mutex_.unlock();
            return false;
        }

        return false;
    }

    bool TryPop(std::vector<Result>& vec_of_results)
    {
        if (queue_mutex_.try_lock())
        {
            if (!queue_.empty())
            {
                vec_of_results.clear(); // for clean result
                while (!queue_.empty())
                {
                    vec_of_results.push_back(queue_.front());
                    queue_.pop();
                }
                queue_mutex_.unlock();
                return true;
            }

            queue_mutex_.unlock();
            return false;
        }

        return false;
    }

private:
    std::mutex queue_mutex_;
    std::queue<Result> queue_;
};

} // namespace tp::exe
