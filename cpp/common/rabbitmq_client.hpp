#pragma once

// std
#include <string>
#include <string_view>
#include <optional>

// rabbitmq
#include <SimpleAmqpClient/SimpleAmqpClient.h>

// nlohmann_json
#include <nlohmann/json.hpp>

//
#include "utils.hpp"

namespace tp {
    
class RabbitClient
{
public:

    struct Message
    {
        nlohmann::json body;
        std::string exchange;
        std::string routing_key;
        utils::DeliveryInfo delivery;
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

    std::string Subscribe(const std::string& queue_name, size_t prefetch_count, bool auto_ack = false);

    void Cancel(const std::string& consumer_tag);

    std::optional<Message> ConsumeOne(const std::string& consumer_tag, int timeout_ms = 100);

    std::vector<Message> ConsumeBatch(const std::string& consumer_tag, size_t max_messages, int timeout_ms = 100);


    void Ack(utils::DeliveryInfo del_info);
    void Reject(utils::DeliveryInfo del_info, bool requeue);

    
    static std::string GetTaskQueueName() { return "task_queue"; }
    static std::string GetResultQueueName() { return "result_queue"; }
    static std::string GetAggregatorQueueName() { return "agregator_queue"; }
    static std::string GetMetricQueueName() { return "metric_queue"; }

private:
    AmqpClient::Channel::ptr_t channel_;

};

} // namespace tp
