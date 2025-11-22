#include "job_factory.hpp"

// std
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>

namespace producer
{
#if 0
static std::string generate_job_id_str()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    std::stringstream stream;
    stream << std::hex;
    stream.width(16);
    stream.fill('0');
    stream << dist(gen);

    return stream.str(); 
}
#endif

static int generate_job_id()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();    
}

JobFactory::jobs_type JobFactory::CreateJob(std::filesystem::path filename, size_t chunk_size)
{
    // for (int i = 0; i < 100; ++i)
    // {
    //     std::cout << generate_job_id() << std::endl;
    // }
    // std::abort();
    int job_id = generate_job_id();
    
    std::cout << "job_id:" << job_id << std::endl;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file:" << filename << std::endl;
        return {};
    }

    jobs_type jobs;
    std::ostringstream current_chunk;
    std::string line;
    size_t lines_in_chunk = 0;
    int section_id = 0;

    while (std::getline(file, line))
    {
        current_chunk << line << "\n";
        lines_in_chunk++;

        if (lines_in_chunk >= chunk_size)
        {
            jobs.push_back(job_type{job_id, section_id++, current_chunk.str(), {}});
            current_chunk.clear();
            lines_in_chunk = 0;
        }
    }

    if (lines_in_chunk > 0)
    {
        jobs.push_back(job_type{job_id, section_id++, current_chunk.str(), {}});
        current_chunk.clear();
        lines_in_chunk = 0;
    }

    return jobs;
}


} // namespace producer