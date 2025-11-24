#pragma once

// common
#include "thread_pool.hpp"

namespace tp::exe
{

template <typename Task>    
void Submit(ThreadPool& pool, Task&& task)
{
    // std::cout << "Submit" << std::endl;
    pool.EnqueueTask(task);
}

} // namespace tp::exe
