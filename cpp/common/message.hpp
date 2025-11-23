#pragma once

// std
#include <string>
#include <vector>

// nlohmann_json
#include <nlohmann/json.hpp>

namespace tp {

using json_type = nlohmann::ordered_json;

struct TaskOption
{
    bool count_word = true;
    bool top_words = true;
    int top_n = 3;

    // utility functions
    static json_type to_json(const TaskOption& task_option);
    static TaskOption from_json(const nlohmann::json& task_option);
};

struct Task
{
    int id;
    int section_id;
    int sections_count;
    std::string filename;
    std::string data;
    TaskOption option;

    // utility function
    static json_type to_json(const Task& task);
    static Task from_json(const nlohmann::json& task);
};

struct Result
{
    Result() = default;
    Result(const Task& task)
        : id{task.id}
        , section_id{task.section_id} 
        , sections_count{task.sections_count} 
        , filename{task.filename} {}

    int id{};
    int section_id{};
    int sections_count{};
    std::string filename;
    std::string data;

    // data
    int word_count{};
    std::vector<std::pair<std::string, int>> top_words;
    
    // utils functions
    static Result merge(const std::vector<Result>& sections);
    static json_type to_statistic_json(const Result& result);
    static json_type to_json(const Result& result);
    static Result from_json(const nlohmann::json& result);
};

struct JobStatus
{
    JobStatus() = default;
    JobStatus(Result result)
        : init{true}
        , id{result.id}
        , sections_count{result.sections_count}
        , received_sections{0} 
        {
            sections_.resize(sections_count);
        }
    bool init = false;
    int id;
    int sections_count{};
    int received_sections{};
    std::vector<Result> sections_;
};

} // namespace tp