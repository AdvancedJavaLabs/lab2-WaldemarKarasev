// std
#include <iostream>

#include <common/message.hpp>
#include <common/rabbitmq_client.hpp>

int main(int argc, char const *argv[])
{
    try
    {
        tp::client::RabbitClient client("localhost");

        tp::message::Task task;

        task.id = 0;
        task.section_id = 0;
        task.text = "Hello RabbitMQ brocker!";
        task.option.count_word = true;
        task.option.top_words = true;
        task.option.top_count = 10;

        nlohmann::json j_task = tp::message::Task::to_json(task);

        std::cout << "Ready to send task" << std::endl;
        client.PublishMessage("text_task", j_task);
        std::cout << "Task was sended to server" << std::endl;

    }
    catch(const std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
