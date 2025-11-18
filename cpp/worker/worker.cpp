#include "worker.hpp"

// std
#include <iostream>

// common
#include <common/message.hpp>
#include <common/thread_pool/task.hpp>
#include <common/thread_pool/submit.hpp>

namespace tp {

Worker::Worker()
    : pool_{std::thread::hardware_concurrency()}
    , client_{}
{

}

int Worker::Run()
{
    while (not stop_flag_)
    {
        std::cout << "===============================" << std::endl;
        auto message = client_.GetFromQueue(tp::client::RabbitClient::GetTaskQueueName());

        if (not message.has_value())
        {
            Result result;
            while (results_.TryPop(result))
            {
                if (result.succeeded)
                {
                    client_.Ack(result.msg.delivery_tag, result.msg.delivery_channel);
                    client_.PublishToQueue(tp::client::RabbitClient::GetAggregatorQueueName(), message::Result::to_json(result.result));
                }
                else
                {
                    client_.Reject(result.msg.delivery_tag, result.msg.delivery_channel, true);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            continue;
        }

        // std::cout << "i:" << i << " -> succeeded to receive task" << std::endl;

        auto task = tp::message::Task::from_json(message.value().body);

        std::cout << "Received task(id:" << task.id << "; section_id:" << task.section_id << ")" << std::endl;

        // if (i % 2 == 0)
        // {
        //     std::cout << "Ack task(id:" << task.id << "; section_id:" << task.section_id << ") and sending it to AgregatorQueue" << std::endl;
        //     client_.Ack(message.value().delivery_tag, message.value().delivery_channel);

        //     tp::message::Result result{task.id, task.section_id, task.data, 100, {{"lol", 10}}};

        //     client_.PublishToQueue(tp::client::RabbitClient::GetAggregatorQueueName(), tp::message::Result::to_json(result));
        // }
        // else
        // {
        //     std::cout << "Rejecting task(id:" << task.id << "; section_id:" << task.section_id << ")" << std::endl;
        //     client_.Reject(message.value().delivery_tag, message.value().delivery_channel, true);
        // }
    }
}
    
} // namespace tp





