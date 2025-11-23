#include "worker.hpp"

// std
#include <iostream>
#include <functional>

// common
#include <common/message.hpp>
#include <common/thread_pool/submit.hpp>

namespace worker {

App::App()
    : tp::RabbitApp{tp::RabbitClient::GetTaskQueueName(), tp::RabbitClient::GetAggregatorQueueName()}
    , pool_{std::thread::hardware_concurrency()} {}

void App::ResultProcessing(tp::RabbitClient& client)
{
    Work::Result result;
    while (results_.TryPop(result))
    {
        if (result.succeeded)
        {
            client.Ack(result.del_info);
            client.PublishToQueue(tp::RabbitClient::GetAggregatorQueueName(), tp::Result::to_json(result.result));
        }
        else
        {
            client.Reject(result.del_info, true);
        }
    }
}

void App::MessageProcessing(tp::RabbitClient::Message msg)
{
    std::cout << "!" << std::endl;
    Work work{
        msg.delivery
        , tp::Task::from_json(msg.body)
        , results_
    };
    std::cout << "!!" << std::endl;

    tp::exe::Submit(pool_, work);
}

    
} // namespace worker





