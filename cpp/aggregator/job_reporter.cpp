#include "job_reporter.hpp"

// std
#include <fstream>
#include <iostream>
#include <filesystem>

namespace aggregator {

static std::filesystem::path dir = "results";

JobReporter::JobReporter(JobResult result)
    : result_{std::move(result)} 
    {
        if (!std::filesystem::exists(dir))
        {
            std::filesystem::create_directory(dir);
        }
    }

void JobReporter::Run()
{
    {
        // creating a report ...
        std::filesystem::path filename = dir 
                                        / std::filesystem::path(std::string("report_id_") + std::to_string(result_.id));

        std::ofstream file(filename);


        if (file.is_open())
        {
            tp::json_type statictics;
            statictics["id"] = result_.id;
            statictics["section_id"] = result_.section_id;
            statictics["sections_count"] = result_.sections_count;
            statictics["file"] = result_.filename;
            statictics["stats"] = tp::TextAnalysisResult::to_json(result_.text_analysis_result, false);
            file << statictics.dump(4);
        }
        else
        {
            std::cout << "failed to open: " << filename.string() << std::endl;
        }
    }

    if (!result_.text_analysis_result.modified_text.empty())
    {
        // printing file data into new file
        std::filesystem::path filename = dir / std::filesystem::path(result_.filename + ".changed");

        std::ofstream file(filename);
        if (file.is_open())
        {
            file << result_.text_analysis_result.modified_text;
        }
        else
        {
            std::cout << "failed to open: " << filename.string() << std::endl;
        }
    }
    else
    {
        std::cout << "modified text is empty" << std::endl;
    }
}


} // namespace aggregator