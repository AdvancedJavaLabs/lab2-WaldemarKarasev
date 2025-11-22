#include "merge_tool.hpp"


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

}

} // namespace worker