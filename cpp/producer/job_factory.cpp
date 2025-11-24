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

static tp::TaskOption ReadOptions(std::filesystem::path config_filename)
{
    tp::TaskOption options{};


    std::ifstream file(config_filename);
    if (!file.is_open())
    {
        std::cerr << "Unable read config file:" << config_filename << std::endl;
        return options;
    }

    tp::json_type config;
    file >> config;

    std::cout << "Config:" << config.dump(4) << std::endl;
    
    options.count_word = config["count_word"];
    options.top_words = config["top_words"];
    options.top_n = config["top_n"];

    options.sentiment = config["sentiment"];
    options.modify_text = config["modify_text"];
    options.mask = config["mask"];
    options.sort_sentences = config["sort_sentences"];

    return options;
}

enum class SeparationType
{
    Lines,
    Sentences,
};

static SeparationType ReadSeparationType(std::filesystem::path config_filename)
{
    SeparationType type = SeparationType::Lines;


    std::ifstream file(config_filename);
    if (!file.is_open())
    {
        std::cerr << "Unable read config file:" << config_filename << std::endl;
        return type;
    }

    tp::json_type config;
    file >> config;

    std::string type_str = config["chunk_type"];
    
    if (type_str == "sentences")
    {
        type = SeparationType::Sentences;
    }

    return type;
}



JobFactory::jobs_type JobFactory::CreateJob(std::filesystem::path config_filename, std::filesystem::path filename, size_t chunk_size)
{
    // for (int i = 0; i < 100; ++i)
    // {
    //     std::cout << generate_job_id() << std::endl;
    // }
    // std::abort();

    if (chunk_size <= 0) chunk_size = 1;

    int job_id = generate_job_id();
    std::cout << "job_id:" << job_id << std::endl;

    tp::TaskOption task_option = ReadOptions(config_filename);
    SeparationType type = ReadSeparationType(config_filename);


    switch (type)
    {
    default:
    case SeparationType::Lines: return SeparateLine(job_id, filename, chunk_size, task_option);

    case SeparationType::Sentences: return SeparateSentences(job_id, filename, chunk_size, task_option);
    
    }

    // unreachable
    return SeparateLine(job_id, filename, chunk_size, task_option);
}

JobFactory::jobs_type JobFactory::SeparateSentences(int job_id
                                                    , std::filesystem::path filename
                                                    , size_t chunk_size
                                                    , tp::TaskOption task_option)
{
    std::cout << "SENTENCES SEPARATION" << std::endl;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file:" << filename << std::endl;
        return {};
    }
    std::cout << "filename:" << filename.filename() << std::endl;

    jobs_type jobs;
    std::ostringstream current_chunk;
    std::string line;
    size_t sentences_in_chunk = 0;
    int section_id = 0;
    while (std::getline(file, line))
    {
        line.push_back('\n');

        for (char c : line)
        {
            current_chunk << c;

            if (c == '.' || c == '!' || c == '?')
            {
                sentences_in_chunk++;

                if (sentences_in_chunk >= chunk_size)
                {
                    // std::cout << "current_chunk:" << current_chunk.str() << std::endl;
                    jobs.push_back(job_type{
                        job_id
                        , section_id++
                        , 0
                        , filename.filename()
                        , current_chunk.str()
                        , task_option
                    });
                    
                    current_chunk.str("");
                    current_chunk.clear();
                    sentences_in_chunk = 0;
                }
            }
        }
    }

    if (sentences_in_chunk > 0)
    {
        jobs.push_back(job_type{
                job_id
                , section_id++
                , 0
                , filename.filename()
                , current_chunk.str()
                , task_option
            });
        current_chunk.clear();
        sentences_in_chunk = 0;
    }

    // setting up sections count for each job
    std::cout << "aaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
    int size = jobs.size();
    for (auto& job : jobs)
    {
        job.sections_count = size;
        // std::cout << tp::Task::to_json(job).dump(4) << std::endl;
    }

    return jobs;
}

JobFactory::jobs_type JobFactory::SeparateLine(int job_id
                                            , std::filesystem::path filename
                                            , size_t chunk_size
                                            , tp::TaskOption task_option)
{
    std::cout << "LINES SEPARATION" << std::endl;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Unable to open file:" << filename << std::endl;
        return {};
    }
    std::cout << "filename:" << filename.filename() << std::endl;

    jobs_type jobs;
    std::ostringstream current_chunk;
    std::string line;
    size_t lines_in_chunk = 0;
    int section_id = 0;
    while (std::getline(file, line))
    {
        current_chunk << line << "\n";
        // std::cout << "line:" << line << std::endl;
        lines_in_chunk++;

        if (lines_in_chunk >= chunk_size)
        {
            // std::cout << "current_chunk:" << current_chunk.str() << std::endl;
            jobs.push_back(job_type{
                job_id
                , section_id++
                , 0
                , filename.filename()
                , current_chunk.str()
                , task_option
            });
            
            current_chunk.str("");
            current_chunk.clear();
            // line.clear();
            lines_in_chunk = 0;
        }
    }

    if (lines_in_chunk > 0)
    {
        jobs.push_back(job_type{
                job_id
                , section_id++
                , 0
                , filename.filename()
                , current_chunk.str()
                , task_option
            });
        current_chunk.clear();
        lines_in_chunk = 0;
    }

    // setting up sections count for each job
    int size = jobs.size();
    for (auto& job : jobs)
    {
        job.sections_count = size;
        // std::cout << tp::Task::to_json(job).dump(4) << std::endl;
    }

    return jobs;
}

} // namespace producer


/**
 *     
 * for (char c : line)
        {
            current_chunk << c;

            // конец предложения по простому признаку
            if (c == '.' || c == '!' || c == '?')
            {
                ++sentences_in_chunk;

                if (sentences_in_chunk >= chunk_size)
                {
                    jobs.push_back(job_type{
                        job_id,
                        section_id++,
                        0,
                        filename.filename(),
                        current_chunk.str(),
                        task_option
                    });

                    current_chunk.str("");
                    current_chunk.clear();
                    sentences_in_chunk = 0;
                }
            }
        }
 * 
 */