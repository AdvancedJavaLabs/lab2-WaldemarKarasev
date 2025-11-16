#pragma once

// std
#include <string>
#include <vector>

// nlohmann_json
#include <nlohmann/json.hpp>

namespace tp::message {

struct TaskOption
{
    bool count_word = true;
    bool top_words = true;
    int top_count = 3;

    // utility functions
    static nlohmann::json to_json(const TaskOption& task_option);
    static TaskOption from_json(const nlohmann::json& task_option);
};

struct Task
{
    int id;
    int section_id;
    std::string text;
    TaskOption option;

    // utility function
    static nlohmann::json to_json(const Task& task);
    static Task from_json(const nlohmann::json& task);
};

struct Result
{
    int id;
    int section_id;
    std::string text;

    // data
    int word_count;
    std::vector<std::pair<std::string, int>> top_words;

    // utils functions
    static nlohmann::json to_json(const Result& result);
    static Result from_json(const nlohmann::json& result);
};
    
} // namespace tp::message