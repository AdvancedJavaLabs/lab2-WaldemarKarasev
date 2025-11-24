// std
#include <iostream>
#include <filesystem>

#include <common/message.hpp>
#include <common/rabbitmq_client.hpp>

#include "job_factory.hpp"

int main(int argc, char const *argv[])
{
    try
    {
        if (argc < 4)
        {

            std::cout << "Usage: " << argv[0] << "<config> <filename> <chunk_size>" << std::endl;
            return 1;
        }

        std::filesystem::path config_file = argv[1];
        std::filesystem::path file = argv[2];
        size_t chunk_size = std::stoul(argv[3]);
        std::cout << "config_file:" << config_file  << "; file:" << file << "; chunk_size:" << chunk_size << std::endl;

        // generating jobs
        auto jobs = producer::JobFactory::CreateJob(config_file, file, chunk_size);
        
        if (jobs.empty())
        {
            return 1;
        }

        // sending jobs to the task queue
        tp::RabbitClient client("localhost");

        std::cout << "Started sending jobs:" << jobs.size() << " to the task queue" << std::endl;
        for (const auto& job : jobs)
        {
            client.PublishToQueue(tp::RabbitClient::GetTaskQueueName(), tp::Task::to_json(job));
        }
        std::cout << "Finished sending jobs to the task queue" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
