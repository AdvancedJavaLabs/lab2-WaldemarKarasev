#include "job_reporter.hpp"


namespace aggregator {

JobReporter::JobReporter(JobResult result)
    : result_{std::move(result)} {}

void JobReporter::Run()
{
    // creating a report ...
}


} // namespace aggregator