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

    bool sentiment = true;

    bool modify_text = true;
    std::string mask{};

    bool sort_sentences = true;

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


struct WordFreq
{
    std::string word;
    size_t count = 0;
};

enum class SentimentLabel : int
{
    Neutral,
    Negative,
    Positive,
};

inline std::string ToString(SentimentLabel label)
{
    switch (label)
    {
    default:
    case SentimentLabel::Neutral: return "Neutral";
    case SentimentLabel::Negative: return "Negative";
    case SentimentLabel::Positive: return "Positive";
    }

    // Unreachable
    return "ErrorLable";
}

inline SentimentLabel FromString(std::string label_str)
{
    if (label_str == "Positive")
    {
        return SentimentLabel::Positive;
    }
    
    if (label_str == "Negative")
    {
        return SentimentLabel::Negative;
    }

    return SentimentLabel::Neutral;
}

struct SentimentStats
{
    int score = 0;
    size_t positive_count = 0;
    size_t negative_count = 0;
    SentimentLabel label = SentimentLabel::Neutral;

    static json_type to_json(const SentimentStats& stats);
    static SentimentStats from_json(const json_type& j_stats);

};

struct TextAnalysisResult
{
    size_t word_count = 0;
    std::vector<WordFreq> top_words;
    SentimentStats sentiment_stats;
    std::string modified_text;
    std::vector<std::string> sorted_sentences;

    static json_type to_json(const TextAnalysisResult& text_stats, bool add_text = true);
    static TextAnalysisResult from_json(const nlohmann::json& j_text_stats);
};

struct Result
{
    Result() = default;
    Result(const Task& task)
        : id{task.id}
        , section_id{task.section_id} 
        , sections_count{task.sections_count} 
        , filename{task.filename} {}

    Result(const Task& task, std::string data, TextAnalysisResult text_stats)
        : id{task.id}
        , section_id{task.section_id} 
        , sections_count{task.sections_count} 
        , filename{task.filename} 
        , data{std::move(data)}
        , text_analysis_result{std::move(text_stats)} {}

    int id{};
    int section_id{};
    int sections_count{};
    std::string filename;
    std::string data;

    TextAnalysisResult text_analysis_result;
    
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