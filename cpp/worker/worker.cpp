#include "worker.hpp"

// std
#include <iostream>
#include <functional>

// common
#include <common/message.hpp>
#include <common/thread_pool/submit.hpp>

namespace worker {

App::App()
    : pool_{std::thread::hardware_concurrency()}
    , client_{} {}

int App::Run()
{
    while (not stop_flag_)
    {
        std::cout << "===============================" << std::endl;
        auto message = client_.GetFromQueue(tp::RabbitClient::GetTaskQueueName());
        if (not message.has_value())
        {
            Work::Result result;
            while (results_.TryPop(result))
            {
                if (result.succeeded)
                {
                    client_.Ack(result.del_info);
                    client_.PublishToQueue(tp::RabbitClient::GetAggregatorQueueName(), tp::Result::to_json(result.result));
                }
                else
                {
                    client_.Reject(result.del_info, true);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            continue;
        }
        std::cout << "!" << std::endl;

        Work work{
            message.value().delivery
            , tp::Task::from_json(message.value().body)
            , results_
        };
        std::cout << "!!" << std::endl;

        tp::exe::Submit(pool_, work);
    }

    return 0;
}
    
} // namespace worker





