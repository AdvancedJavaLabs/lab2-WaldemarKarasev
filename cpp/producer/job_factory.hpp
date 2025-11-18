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
    using jobs_type = std::vector<tp::message::Task>;

public:
    static jobs_type CreateJob(std::filesystem::path filename, size_t chunk_size);
};

} // namespace producer
