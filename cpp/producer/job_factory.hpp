#pragma once

// std
#include <filesystem>
#include <vector>

// tp
#include <common/message.hpp>

namespace producer
{

class JobFactory
{
public:
    using job_type = tp::Task;
    using jobs_type = std::vector<job_type>;

public:
    static jobs_type CreateJob(std::filesystem::path filename, size_t chunk_size);
};

} // namespace producer
