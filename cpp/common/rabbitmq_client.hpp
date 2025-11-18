#pragma once

// std
#include <string>
#include <string_view>
#include <optional>

// rabbitmq
#include <SimpleAmqpClient/SimpleAmqpClient.h>

// nlohmann_json
#include <nlohmann/json.hpp>

namespace tp::client {
    
class RabbitClient
{
public:
    struct Message
    {
        nlohmann::json body;
        std::string exchange;
        std::string routing_key;
        uint64_t delivery_tag;
        uint16_t delivery_channel;
    };

public:
    RabbitClient(std::string host = "localhost", std::string username = "guest", std::string password = "guest", int port = 5672);
    ~RabbitClient() = default;

    void DeclareQueue(const std::string& queue_name
                    , bool durable = true
                    , bool auto_delete = false);

    void PublishToQueue(const std::string& queue_name
                        , const nlohmann::json& payload);

    std::optional<Message> GetFromQueue(const std::string& queue_name
                                        , bool auto_ack = false);

    void Ack(uint64_t delivery_tag, uint16_t delivery_channel);
    void Reject(uint64_t delivery_tag, uint16_t delivery_channel, bool requeue);

    
    static std::string GetTaskQueueName() { return "task_queue"; }
    static std::string GetResultQueueName() { return "result_queue"; }
    static std::string GetAggregatorQueueName() { return "agregator_queue"; }

private:
    AmqpClient::Channel::ptr_t channel_;

};

} // namespace tp::client
