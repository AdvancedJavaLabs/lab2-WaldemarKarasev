#pragma once

#include <common/thread_pool/result_queue.hpp>
#include <common/thread_pool/thread_pool.hpp>
#include <common/rabbitmq_client.hpp>
#include <common/message.hpp>
namespace tp
{

class Worker
{
public:
    struct Result
    {
        bool succeeded = false;
        client::RabbitClient::Message msg; // to ack or reject failed task
        message::Result result;
    };

public:
    Worker();
    int Run();

private:
    exe::ThreadPool pool_;
    client::RabbitClient client_;
    exe::ResultQueue<Result> results_{};
    bool stop_flag_ = false;
};
    
} 

