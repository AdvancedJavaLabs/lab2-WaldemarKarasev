#include "rabbitmq_client.hpp"

// std
#include <thread>

namespace tp {
    
RabbitClient::RabbitClient(std::string host, std::string username, std::string password, int port)
{
    std::string uri = "amqp://" + username + ":" + password + "@" + host + ":" + std::to_string(port);
    channel_ = AmqpClient::Channel::CreateFromUri(std::move(uri));
}


void RabbitClient::DeclareQueue(const std::string& queue_name
                , bool durable
                , bool auto_delete)
{
    channel_->DeclareQueue(queue_name, /*passive*/false, /*durable*/durable, /*exclusive*/false, /**/auto_delete);
}

void RabbitClient::PublishToQueue(const std::string& queue_name
                    , const nlohmann::json& payload)
{
    DeclareQueue(queue_name);

    const std::string body = payload.dump();

    auto message = AmqpClient::BasicMessage::Create(body);

    channel_->BasicPublish("", queue_name, message);
}

std::optional<RabbitClient::Message> RabbitClient::GetFromQueue(const std::string& queue_name
                                    , bool auto_ack)
{
    AmqpClient::Envelope::ptr_t env;
    channel_->BasicGet(env, queue_name, auto_ack);

    if (env == nullptr)
    {
        return std::nullopt;
    }

    Message msg;
    msg.body = nlohmann::json::parse(env->Message()->Body());
    msg.exchange = env->Exchange();
    msg.routing_key = env->RoutingKey();
    
    // getting delivery info
    msg.delivery = {
        env->GetDeliveryInfo().delivery_tag
        , env->GetDeliveryInfo().delivery_channel
    };

    return msg;
}

std::string RabbitClient::Subscribe(const std::string& queue_name, size_t prefetch_count, bool auto_ack)
{
    DeclareQueue(queue_name);

    bool no_ack = auto_ack;

    std::string consumer_tag = channel_->BasicConsume(
        queue_name
        , /*consumer_tag=*/""
        , /*no_local=*/false
        , /*no_ack=*/no_ack
        , /*exclusive=*/false
        , prefetch_count
    );


    return consumer_tag;
}

void RabbitClient::Cancel(const std::string& consumer_tag)
{
    channel_->BasicCancel(consumer_tag);
}

std::optional<RabbitClient::Message>
RabbitClient::ConsumeOne(const std::string& consumer_tag
                        , int timeout_ms)
{
    AmqpClient::Envelope::ptr_t env;

    bool got = channel_->BasicConsumeMessage(consumer_tag, env, timeout_ms);
    if (!got) 
    {
        return std::nullopt;
    }

    Message msg;
    msg.body        = nlohmann::json::parse(env->Message()->Body());
    msg.exchange    = env->Exchange();
    msg.routing_key = env->RoutingKey();

    msg.delivery = {
            env->GetDeliveryInfo().delivery_tag
            , env->GetDeliveryInfo().delivery_channel
        };

    return msg;
}

std::vector<RabbitClient::Message>
RabbitClient::ConsumeBatch(const std::string& consumer_tag, size_t max_messages, int timeout_ms)
{
    std::vector<Message> result;
    result.reserve(max_messages);

    for (std::size_t i = 0; i < max_messages; ++i) 
    {
        int per_call_timeout = (i == 0 ? timeout_ms : 0);

        auto msg_opt = ConsumeOne(consumer_tag, per_call_timeout);
        if (!msg_opt) 
        {
            break;
        }
        result.push_back(std::move(*msg_opt));
    }

    return result;
}

void RabbitClient::Ack(utils::DeliveryInfo del_info)
{
    channel_->BasicAck({del_info.tag, del_info.channel});
}

void RabbitClient::Reject(utils::DeliveryInfo del_info, bool requeue)
{
    channel_->BasicReject({del_info.tag, del_info.channel}, requeue);
}

} // namespace tp
