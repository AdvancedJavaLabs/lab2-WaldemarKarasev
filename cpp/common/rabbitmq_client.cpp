#include "rabbitmq_client.hpp"

namespace tp::client {
    
RabbitClient::RabbitClient(std::string host, std::string username, std::string password, int port)
{
    std::string uri = "amqp://" + username + ":" + password + "@" + host + ":" + std::to_string(port);

    channel_ = AmqpClient::Channel::CreateFromUri(std::move(uri));
}

void RabbitClient::PublishMessage(const std::string queue_name, const nlohmann::json& message)
{
    auto body = message.dump();

    auto msg = AmqpClient::BasicMessage::Create(body);

    channel_->DeclareQueue(queue_name, false, true, false, false);
    channel_->BasicPublish("", queue_name, msg);
}

nlohmann::json RabbitClient::ConsumeMessage(const std::string queue_name)
{
    channel_->DeclareQueue(queue_name, false, true, false, false);

    std::string consumer_tag = channel_->BasicConsume(queue_name, "");
    AmqpClient::Envelope::ptr_t env;

    channel_->BasicConsumeMessage(consumer_tag, env);
    channel_->BasicCancel(consumer_tag);

    const std::string& body = env->Message()->Body();
    
    return nlohmann::json::parse(body);
}

} // namespace tp::client
