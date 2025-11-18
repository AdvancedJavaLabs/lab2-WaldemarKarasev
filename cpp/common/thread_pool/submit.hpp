#pragma once

// common
#include "thread_pool.hpp"
#include "result_queue.hpp"
#include "task.hpp"

// std
#include <functional>

namespace tp::exe
{

template <typename T>    
void Submit(ThreadPool& pool, ResultQueue<T>& results, ITask<T>& task)
{
    pool.EnqueueTask([results, task](){
        T result = task();        
        results.Push(std::move(task));
    });
}

} // namespace tp::exe
