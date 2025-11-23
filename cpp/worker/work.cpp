#include "work.hpp"

namespace worker {

Work::Work(DeliveryInfoType del_info, Task task, ResultQueue& queue)
    : task_{std::move(task)}
    , del_info_{std::move(del_info)}
    , result_queue_{queue} 
    {
        std::cout << "Work()" << std::endl;
    }

Work::~Work()
{
    std::cout << "~Work()" << std::endl;
}

void Work::Run()
{
    
    Result work_result;
    work_result.succeeded = true;
    work_result.del_info = del_info_;
    
    {
        tp::Result task_result(task_);
        // std::cout << tp::Result::to_json(task_result).dump(4) << std::endl;
        task_result.data = std::move(task_.data);
        work_result.result = std::move(task_result);
    }
    
    
    // text processing
    // ...
    
    
    std::cout << "Work ended: " << "id:" << task_.id << "; section_id:" << task_.section_id << std::endl;
    result_queue_.Push(std::move(work_result));
}

// private:
//     Task task;
//     DeliveryInfoType delivery_info;
//     ResultQueue& queue;
// };

} // namespace worker