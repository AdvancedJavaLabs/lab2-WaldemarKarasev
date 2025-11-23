#pragma once

#include <common/thread_pool/result_queue.hpp>
#include <common/thread_pool/thread_pool.hpp>
#include <common/rabbitmq_client.hpp>
#include <common/app.hpp>

#include "work.hpp"

namespace worker {

class App final : public tp::RabbitApp
{
public:

public:
    App();
    
private:
    virtual void ResultProcessing(tp::RabbitClient& client) override;
    virtual void MessageProcessing(tp::RabbitClient::Message message) override;

private:
    tp::exe::ThreadPool pool_;
    tp::exe::ResultQueue<Work::Result> results_{};
};
    
} // namespace worker

