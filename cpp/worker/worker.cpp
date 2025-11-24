#include "worker.hpp"

// std
#include <iostream>
#include <functional>

// common
#include <common/message.hpp>
#include <common/thread_pool/submit.hpp>

namespace worker {

App::App(size_t prefetch_count)
    : tp::RabbitApp{prefetch_count, tp::RabbitClient::GetTaskQueueName(), tp::RabbitClient::GetAggregatorQueueName()}
    , pool_{std::thread::hardware_concurrency()} {}

void App::ResultProcessing(tp::RabbitClient& client)
{
    std::vector<Work::Result> results_vec;
    while (results_.TryPop(results_vec))
    {
        for (const auto& result : results_vec)
        {
            std::cout << "Ready results:" << results_vec.size() << std::endl;
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
}

void App::MessageProcessing(tp::RabbitClient::Message msg)
{
    // std::cout << "!" << std::endl;
    Work work{
        msg.delivery
        , tp::Task::from_json(msg.body)
        , results_
    };
    // std::cout << "!!" << std::endl;

    tp::exe::Submit(pool_, work);
}

void App::MessageProcessing(std::vector<tp::RabbitClient::Message> messages)
{
    for (auto& message : messages)
    {
        MessageProcessing(std::move(message));
    }
}
    
} // namespace worker





