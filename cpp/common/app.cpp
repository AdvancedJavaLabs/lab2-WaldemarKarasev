#include "app.hpp"

// std
#include <iostream>

namespace tp {
    
RabbitApp::RabbitApp(size_t prefetch_count, std::string read_queue, std::string write_queue)
    : stop_flag_{false}
    , read_queue_name_{std::move(read_queue)}
    , write_queue_name_{std::move(write_queue)}
    , prefetch_count_{prefetch_count} {}


int RabbitApp::Run()
{
    try
    {   
        int responce_wait_count = 0;
        std::string consumer_tag = client_.Subscribe(read_queue_name_, prefetch_count_);
        while (!stop_flag_)
        {
            // std::cout << "===============================" << std::endl
            auto messages = client_.ConsumeBatch(consumer_tag, std::thread::hardware_concurrency() * 2);
            if (messages.empty() || responce_wait_count > 20)
            {
                ResultProcessing(client_);
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                
                // resetting wait count
                if (responce_wait_count > 40)
                {
                    responce_wait_count = 0;
                }
                continue;
            }

            MessageProcessing(messages);
            // std::this_thread::sleep_for();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "RabbitApp:::exception:" << e.what() << '\n';
        return 1;
    }

    return 0;
}

} // namespace tp
