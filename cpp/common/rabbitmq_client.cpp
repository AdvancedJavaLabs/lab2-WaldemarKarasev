#include "rabbitmq_client.hpp"

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

void RabbitClient::Ack(utils::DeliveryInfo del_info)
{
    channel_->BasicAck({del_info.tag, del_info.channel});
}

void RabbitClient::Reject(utils::DeliveryInfo del_info, bool requeue)
{
    channel_->BasicReject({del_info.tag, del_info.channel}, requeue);
}

} // namespace tp
