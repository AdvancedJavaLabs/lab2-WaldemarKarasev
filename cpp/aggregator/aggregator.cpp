#include "aggregator.hpp"

// std
#include <iostream>

// ...
#include "job_reporter.hpp"

namespace aggregator {
    
int App::Run()
{
    while (true)
    {
        std::cout << "===============================" << std::endl;
        auto message = client_.GetFromQueue(tp::RabbitClient::GetAggregatorQueueName());
        if (not message.has_value())
        {
            // sending responces
            std::vector<MergeTool::Responce> responces_vec;
            if (responce_queue_.TryPop(responces_vec))
            {
                for (const auto& responce : responces_vec)
                {
                    if (responce.succeeded)
                    {
                        client_.Ack(responce.del_info);
                    }
                    else
                    {
                        client_.Reject(responce.del_info, true);
                    }
                }
            }

            std::vector<MergeTool::Result> results_vec;
            if (result_queue_.TryPop(results_vec))
            {
                for (auto& result : results_vec)
                {
                    // result processing ...
                    // Submit to pool result -> print changed file and report
                    tp::exe::Submit(pool_, JobReporter{std::move(result)});
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            continue;
        }
        std::cout << "!" << std::endl;

        MergeTool work{
            message.value().delivery
            , tp::Result::from_json(message.value().body)
            , responce_queue_
            , result_queue_
            , merge_table_
        };
        std::cout << "!!" << std::endl;

        tp::exe::Submit(pool_, work);
    }

    return 0;
}


} // namespace aggregator
