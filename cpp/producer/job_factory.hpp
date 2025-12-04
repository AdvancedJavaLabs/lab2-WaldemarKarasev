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
    static jobs_type CreateJob(std::filesystem::path config_filename, std::filesystem::path filename, size_t chunk_size);
    static jobs_type SeparateSentences(int job_id
                                    , std::filesystem::path filename
                                    , size_t chunk_size
                                    , tp::TaskOption task_option);
                                    
    static jobs_type SeparateLine(int job_id
                                , std::filesystem::path filename
                                , size_t chunk_size
                                , tp::TaskOption task_option);
};

} // namespace producer
