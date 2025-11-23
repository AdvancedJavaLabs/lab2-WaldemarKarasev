#pragma once

// std
#include <unordered_map>

// common
#include <common/thread_pool/task.hpp>
#include <common/thread_pool/submit.hpp>
#include <common/thread_pool/result_queue.hpp>
#include <common/utils.hpp>
#include <common/message.hpp>

namespace aggregator {

struct MergeTable
{
    std::unordered_map<int, tp::JobStatus> table_;
    std::mutex table_mutex_;
};

class MergeTool : public tp::exe::ITask
{
public:
    struct Responce
    {
        bool succeeded = false;
        tp::utils::DeliveryInfo del_info; // to ack or reject failed task
    };

public:
    using DeliveryInfoType = tp::utils::DeliveryInfo;
    using Result = tp::Result;
    using ResultQueue = tp::exe::ResultQueue<Result>;
    using ResponceQueue = tp::exe::ResultQueue<Responce>;

public:
    MergeTool(DeliveryInfoType del_info
            , Result result
            , ResponceQueue& responce_queue
            , ResultQueue& result_queue
            , MergeTable& merge_table);
    ~MergeTool();

protected:
    virtual void Run() override;

private:
    Result MergeJobStatus(tp::JobStatus job_status);

private:
    DeliveryInfoType del_info_;
    Result result_;
    ResponceQueue& responce_queue_;
    ResultQueue& result_queue_;
    MergeTable& merge_table_;
};

} // namespace worker