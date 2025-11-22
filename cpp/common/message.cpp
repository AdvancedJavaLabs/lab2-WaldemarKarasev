#include "message.hpp"

namespace tp {

// utility functions
json_type TaskOption::to_json(const TaskOption& task_option)
{
    json_type j_task_option;
    j_task_option["w_count"] = task_option.count_word;
    j_task_option["top"] = task_option.top_words;
    j_task_option["top_n"] = task_option.top_n;
    return j_task_option;
}

TaskOption TaskOption::from_json(const nlohmann::json& j_task_option)
{
    TaskOption task_option;
    task_option.count_word = j_task_option["w_count"];
    task_option.top_words = j_task_option["top"];
    task_option.top_n = j_task_option["top_n"];
    return task_option;
}

json_type Task::to_json(const Task& task)
{
    json_type j_task;
    j_task["id"] = task.id;
    j_task["section_id"] = task.section_id;
    j_task["data"] = task.data;
    j_task["option"] = TaskOption::to_json(task.option);
    return j_task;
}

Task Task::from_json(const nlohmann::json& j_task)
{
    Task task;
    task.id = j_task["id"]; 
    task.section_id = j_task["section_id"]; 
    task.data = j_task["data"]; 
    task.option = TaskOption::from_json(j_task["option"]); 
    return task;
}

json_type Result::to_json(const Result& result)
{
    json_type j_result;
    j_result["id"] = result.id;
    j_result["section_id"] = result.section_id;

    json_type j_results;
    j_results["word_count"] = result.word_count;
    {
        nlohmann::json top_arr = nlohmann::json::array();
        for (const auto& elem : result.top_words)
        {
            nlohmann::json pair;
            pair["word"] = elem.first;
            pair["count"] = elem.second;
            top_arr.push_back(pair);
        }
        j_results["top_words"] = top_arr;
    }
    j_result["result"] = j_results;
    j_result["data"] = result.data;

    return j_result;
}

Result Result::from_json(const nlohmann::json& j_result)
{
    Result result;
    result.id = j_result["id"];
    result.section_id = j_result["section_id"];
    result.word_count = j_result["result"]["word_count"];

    for (const nlohmann::json& obj : j_result["result"]["top_words"])
    {
        result.top_words.push_back({obj["word"], obj["count"]});
    }
    result.data = j_result["data"];

    return result;
}


} // namespace tp