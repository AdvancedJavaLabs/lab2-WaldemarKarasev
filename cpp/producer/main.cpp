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
        if (argc < 3)
        {

            std::cout << "Usage: " << argv[0] << " <filename> <chunk_size>" << std::endl;
            return 1;
        }

        std::filesystem::path file = argv[1];
        size_t chunk_size = std::stoul(argv[2]);
        std::cout << "file:" << file << "; chunk_size:" << chunk_size << std::endl;

        // generating jobs
        auto jobs = producer::JobFactory::CreateJob(file, chunk_size);
        
        if (jobs.empty())
        {
            return 1;
        }

        // sending jobs to the task queue
        tp::client::RabbitClient client("localhost");

        std::cout << "Started sending jobs:" << jobs.size() << " to the task queue" << std::endl;
        for (const auto& job : jobs)
        {
            client.PublishToQueue(tp::client::RabbitClient::GetTaskQueueName(), tp::message::Task::to_json(job));
        }
        std::cout << "Finished sending jobs to the task queue" << std::endl;

        #if 0
        {            
            tp::message::Task task;
    
            task.id = 0;
            task.section_id = 0;
            task.data = "Hello RabbitMQ brocker!";
            task.option.count_word = true;
            task.option.top_words = true;
            task.option.top_n = 10;
    
            nlohmann::json j_task = tp::message::Task::to_json(task);
    
            std::cout << "Ready to send task" << std::endl;
            client.PublishMessage("text_task", j_task);
            std::cout << "Task was sended to server" << std::endl;
        }
        #endif

    }
    catch(const std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
