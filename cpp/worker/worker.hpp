#pragma once

#include <common/thread_pool/result_queue.hpp>
#include <common/thread_pool/thread_pool.hpp>
#include <common/rabbitmq_client.hpp>

#include "work.hpp"

namespace worker {

class App
{
public:

public:
    App();
    int Run();

private:
    tp::exe::ThreadPool pool_;
    tp::RabbitClient client_;
    tp::exe::ResultQueue<Work::Result> results_{};
    bool stop_flag_ = false;
};
    
} // namespace worker

