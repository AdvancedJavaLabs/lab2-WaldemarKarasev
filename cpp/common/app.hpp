#pragma once

// std
#include <string>

// common
#include "thread_pool/thread_pool.hpp"
#include "rabbitmq_client.hpp"

namespace tp
{

class RabbitApp
{
public:
    RabbitApp(size_t prefetch_count, std::string read_queue, std::string write_queue);
    int Run();

protected:
    virtual void ResultProcessing(RabbitClient& client) = 0;
    virtual void MessageProcessing(RabbitClient::Message mgs) = 0;
    virtual void MessageProcessing(std::vector<RabbitClient::Message> mgs) = 0;


private:
    bool stop_flag_ = false;
    std::string read_queue_name_;
    std::string write_queue_name_;
    RabbitClient client_;
    size_t prefetch_count_;
};

} // namespace tp
