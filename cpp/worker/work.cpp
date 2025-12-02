#include "work.hpp"

#include <common/text_processor.hpp>

namespace worker {

Work::Work(DeliveryInfoType del_info, Task task, ResultQueue& queue)
    : task_{std::move(task)}
    , del_info_{std::move(del_info)}
    , result_queue_{queue} {}

Work::~Work() {}

void Work::Run()
{
    tp::TextProcessor processor{std::move(task_), {"good"}, {"bad"}};

    Result work_result;
    work_result.succeeded = true;
    work_result.del_info = del_info_;
    work_result.result = processor.ProcessText();

    std::cout << "Work ended: " << "id:" << work_result.result.id << "; section_id:" << work_result.result.section_id << std::endl;
    result_queue_.Push(std::move(work_result));
}

} // namespace worker