#include "app.hpp"

// std
#include <iostream>

namespace tp {
    
RabbitApp::RabbitApp(std::string read_queue, std::string write_queue)
    : stop_flag_{false}
    , read_queue_name_{std::move(read_queue)}
    , write_queue_name_{std::move(write_queue)} {}


int RabbitApp::Run()
{
    try
    {   
        int responce_wait_count = 0;
        while (!stop_flag_)
        {
            // std::cout << "===============================" << std::endl;
            auto message = client_.GetFromQueue(read_queue_name_);
            if (not message.has_value() || responce_wait_count > 40)
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

            MessageProcessing(message.value());
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
