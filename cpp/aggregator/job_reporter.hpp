#pragma once

// common
#include <common/thread_pool/task.hpp>
#include <common/message.hpp>



namespace aggregator {

class JobReporter : public tp::exe::ITask
{
public:
    using JobResult = tp::Result;

public:
    JobReporter(JobResult result);

    virtual void Run() override;

private:
    JobResult result_;
};

} // namespace aggregator