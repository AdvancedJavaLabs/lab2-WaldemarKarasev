#pragma once

// std
#include <mutex>

// common
#include <common/message.hpp>
#include <common/thread_pool/thread_pool.hpp>
#include <common/rabbitmq_client.hpp>
#include <common/thread_pool/result_queue.hpp>
#include <common/app.hpp>

//
#include "merge_tool.hpp"


namespace aggregator {

class App final : public tp::RabbitApp
{
public:
    App(size_t prefetch_count);

private:
    virtual void ResultProcessing(tp::RabbitClient& client) override;
    virtual void MessageProcessing(tp::RabbitClient::Message message) override;
    virtual void MessageProcessing(std::vector<tp::RabbitClient::Message> messages) override;

private:
    tp::exe::ThreadPool pool_;
    MergeTable merge_table_;
    tp::exe::ResultQueue<MergeTool::Responce> responce_queue_;
    tp::exe::ResultQueue<MergeTool::Result> result_queue_;
};

} // namespace aggregator