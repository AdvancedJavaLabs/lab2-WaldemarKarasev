#include "merge_tool.hpp"

// std
#include <mutex>

namespace aggregator {

MergeTool::MergeTool(DeliveryInfoType del_info
                    , Result result
                    , ResponceQueue& responce_queue
                    , ResultQueue& result_queue
                    , MergeTable& merge_table)
                    : del_info_{std::move(del_info)}
                    , result_{std::move(result)}
                    , responce_queue_{responce_queue}
                    , result_queue_{result_queue}
                    , merge_table_{merge_table} {}

MergeTool::~MergeTool()
{

}

void MergeTool::Run()
{
    std::unique_lock lock(merge_table_.table_mutex_);

    if (!merge_table_.table_.contains(result_.id))
    {
        std::cout << "Initialization of job status for id:" << result_.id << "; sections:" << result_.sections_count << std::endl;
        merge_table_.table_[result_.id] = tp::JobStatus{result_};
    }
    tp::JobStatus& job_status = merge_table_.table_[result_.id];
    std::cout << "sections.size=" << job_status.sections_.size() 
    << "; section_id:" << result_.section_id << std::endl;
    job_status.sections_[result_.section_id] = result_;
    job_status.received_sections++;

    if (job_status.received_sections == job_status.sections_count)
    {
        // merging into once result and delete this job status from merge table
        std::cout << "Job ready" << std::endl;
        tp::JobStatus ready_job_status = merge_table_.table_[result_.id];
        merge_table_.table_.erase(result_.id);
        lock.unlock();

        result_queue_.Push(MergeJobStatus(ready_job_status));
    }
    // sending responce to 
    responce_queue_.Push(Responce{true, del_info_});
}

MergeTool::Result MergeTool::MergeJobStatus(tp::JobStatus job_status)
{
    // merging
    Result result = tp::Result::merge(job_status.sections_);
    return result;
}

} // namespace worker