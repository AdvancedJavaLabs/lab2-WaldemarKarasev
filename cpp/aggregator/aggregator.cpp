#include "aggregator.hpp"

// std
#include <iostream>

// ...
#include "job_reporter.hpp"

namespace aggregator {
    
App::App(size_t prefetch_count)
    : tp::RabbitApp(prefetch_count, tp::RabbitClient::GetAggregatorQueueName(), "") {}


void App::ResultProcessing(tp::RabbitClient& client)
{
    // sending responces
    std::vector<MergeTool::Responce> responces_vec;
    if (responce_queue_.TryPop(responces_vec))
    {
        std::cout << "Ready responces:" << responces_vec.size() << std::endl;
        for (const auto& responce : responces_vec)
        {
            if (responce.succeeded)
            {
                client.Ack(responce.del_info);
            }
            else
            {
                client.Reject(responce.del_info, true);
            }
        }

        std::vector<MergeTool::Result> results_vec;
        if (result_queue_.TryPop(results_vec))
        {
            std::cout << "Ready to report works:" << results_vec.size() << std::endl;
            for (auto& result : results_vec)
            {
                // result processing ...
                // Submit to pool result -> print changed file and report
                tp::exe::Submit(pool_, JobReporter{std::move(result)});
            }
        }
    }
}

void App::MessageProcessing(tp::RabbitClient::Message msg)
{
    auto result = tp::Result::from_json(msg.body);
    MergeTool work{
        msg.delivery
        , result
        , responce_queue_
        , result_queue_
        , merge_table_
    };
    std::cout << "WorK SUBMITTED: " << "id:" << result.id << "; section_id:" << result.section_id << std::endl;
    tp::exe::Submit(pool_, work);
}

void App::MessageProcessing(std::vector<tp::RabbitClient::Message> messages)
{
    for (auto& message : messages)
    {
        MessageProcessing(std::move(message));
    }
}

} // namespace aggregator
