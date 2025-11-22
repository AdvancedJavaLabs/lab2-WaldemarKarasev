#pragma once

// std
#include <mutex>

// common
#include <common/message.hpp>
#include <common/thread_pool/thread_pool.hpp>
#include <common/rabbitmq_client.hpp>
#include <common/thread_pool/result_queue.hpp>

//
#include "merge_tool.hpp"


namespace aggregator {

class App
{
public:
    int Run();

private:
    tp::exe::ThreadPool pool_;
    tp::RabbitClient client_;
    MergeTable merge_table_;
    tp::exe::ResultQueue<MergeTool::Responce> responce_queue_;
    tp::exe::ResultQueue<MergeTool::Result> result_queue_;
};

} // namespace aggregator