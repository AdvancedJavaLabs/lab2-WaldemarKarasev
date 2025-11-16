#pragma once

#include "message.hpp"

namespace tp::message {

// utility functions
nlohmann::json TaskOption::to_json(const TaskOption& task_option)
{
    return {};
}

TaskOption TaskOption::from_json(const nlohmann::json& task_option)
{
    return {};
}

nlohmann::json Task::to_json(const Task& task)
{
    return {};
}

Task Task::from_json(const nlohmann::json& task)
{
    return {};
}

nlohmann::json Result::to_json(const Result& result)
{
    return {};
}

Result Result::from_json(const nlohmann::json& result)
{
    return {};
}


} // namespace tp::message