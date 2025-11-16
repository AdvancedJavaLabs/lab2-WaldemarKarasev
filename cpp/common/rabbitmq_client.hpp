#pragma once

// std
#include <string>
#include <string_view>

// rabbitmq
#include <SimpleAmqpClient/SimpleAmqpClient.h>

// nlohmann_json
#include <nlohmann/json.hpp>

namespace tp::client {
    
class RabbitClient
{
public:
    RabbitClient(std::string host, std::string username = "guest", std::string password = "guest", int port = 5672);

    void PublishMessage(const std::string queue_name, const nlohmann::json& message);
    nlohmann::json ConsumeMessage(const std::string queue_name);

private:
    AmqpClient::Channel::ptr_t channel_;

};

} // namespace tp::client
