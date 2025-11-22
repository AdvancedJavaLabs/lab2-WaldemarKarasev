#pragma once

// common
#include <common/thread_pool/task.hpp>
#include <common/thread_pool/submit.hpp>
#include <common/thread_pool/result_queue.hpp>
#include <common/utils.hpp>
#include <common/message.hpp>

namespace worker {

class Work : public tp::exe::ITask
{
public:
    struct Result
    {
        bool succeeded = false;
        tp::utils::DeliveryInfo del_info; // to ack or reject failed task
        tp::Result result;
    };

public:
    using DeliveryInfoType = tp::utils::DeliveryInfo;
    using Task = tp::Task;
    using ResultQueue = tp::exe::ResultQueue<Result>;

public:
    Work(DeliveryInfoType del_info, Task task, ResultQueue& queue);
    ~Work();
protected:
    virtual void Run() override;

private:
    Task task_;
    DeliveryInfoType del_info_;
    ResultQueue& result_queue_;
};

} // namespace worker