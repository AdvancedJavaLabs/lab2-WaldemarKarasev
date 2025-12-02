#include "message.hpp"

// std
#include <queue>
#include <iostream>

namespace tp {

// utility functions
json_type TaskOption::to_json(const TaskOption& task_option)
{
    json_type j_task_option;
    j_task_option["w_count"] = task_option.count_word;
    j_task_option["top"] = task_option.top_words;
    j_task_option["top_n"] = task_option.top_n;
    j_task_option["sentiment"] = task_option.sentiment;
    j_task_option["modify_text"] = task_option.modify_text;
    j_task_option["mask"] = task_option.mask;
    j_task_option["sort_sentences"] = task_option.sort_sentences;
    return j_task_option;
}

TaskOption TaskOption::from_json(const nlohmann::json& j_task_option)
{
    TaskOption task_option;
    task_option.count_word = j_task_option["w_count"];
    task_option.top_words = j_task_option["top"];
    task_option.top_n = j_task_option["top_n"];
    task_option.sentiment = j_task_option["sentiment"];
    task_option.modify_text = j_task_option["modify_text"];
    task_option.mask = j_task_option["mask"];
    task_option.sort_sentences = j_task_option["sort_sentences"];
    return task_option;
}

json_type Task::to_json(const Task& task)
{
    json_type j_task;
    j_task["id"] = task.id;
    j_task["section_id"] = task.section_id;
    j_task["sections_count"] = task.sections_count;
    j_task["file"] = task.filename;
    j_task["data"] = task.data;
    j_task["option"] = TaskOption::to_json(task.option);
    return j_task;
}

Task Task::from_json(const nlohmann::json& j_task)
{
    Task task;
    task.id = j_task["id"]; 
    task.section_id = j_task["section_id"];
    task.sections_count = j_task["sections_count"];
    task.filename = j_task["file"];
    task.data = j_task["data"]; 
    task.option = TaskOption::from_json(j_task["option"]); 
    return task;
}

json_type SentimentStats::to_json(const SentimentStats& stats)
{
    json_type j_stats;
    j_stats["score"] = stats.score;
    j_stats["positive_count"] = stats.positive_count;
    j_stats["negative_count"] = stats.negative_count;
    j_stats["lable"] = ToString(stats.label);
    return j_stats;
}

SentimentStats SentimentStats::from_json(const json_type& j_stats)
{
    SentimentStats stats;

    stats.score = j_stats["score"];
    stats.positive_count = j_stats["positive_count"];
    stats.negative_count = j_stats["negative_count"];
    stats.label = FromString(j_stats["lable"]);

    return stats;
}


json_type TextAnalysisResult::to_json(const TextAnalysisResult& text_stats, bool add_text)
{
    json_type j_text_stats;

    // word count
    j_text_stats["word_count"] = text_stats.word_count;

    // top words
    {
        nlohmann::json top_arr = nlohmann::json::array();
        for (const auto& elem : text_stats.top_words)
        {
            nlohmann::json pair;
            pair["word"] = elem.word;
            pair["count"] = elem.count;
            top_arr.push_back(pair);
        }
        j_text_stats["top_words"] = top_arr;
    }

    // sentiment stats
    {
        j_text_stats["sentiment"] = SentimentStats::to_json(text_stats.sentiment_stats);
    }

    // modified text
    if (add_text)
    {
        j_text_stats["modified_text"] = text_stats.modified_text;
    }

    // sorted_sentences
    {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& sentence : text_stats.sorted_sentences)
        {
            arr.push_back(sentence);
        }

        j_text_stats["sorted_sentences"] = arr;
    }

    return j_text_stats;
}

TextAnalysisResult TextAnalysisResult::from_json(const nlohmann::json& j_text_stats)
{
    TextAnalysisResult text_stats;

    // word count
    text_stats.word_count = j_text_stats["word_count"];

    // top words
    {
        std::vector<WordFreq> top_words;
        for (const auto& elem : j_text_stats["top_words"])
        {
            WordFreq freq;
            freq.word = elem["word"];
            freq.count = elem["count"];
            top_words.push_back(freq);
        }

        text_stats.top_words = std::move(top_words);
    }

    // sentiment stats
    {
        text_stats.sentiment_stats = SentimentStats::from_json(j_text_stats["sentiment"]);
    }

    // modified text
    {
        text_stats.modified_text = j_text_stats["modified_text"];
    }

    // sorted_sentences
    {
        std::vector<std::string> sorted_sentences;
        for (const auto& sentence : j_text_stats["sorted_sentences"])
        {
            sorted_sentences.push_back(sentence);
        }

        text_stats.sorted_sentences = std::move(sorted_sentences);
    }


    return text_stats;
}


Result Result::merge(const std::vector<Result>& sections)
{
    Result merged_result;
    merged_result.id = sections.front().id;
    merged_result.section_id = -1;
    merged_result.sections_count = sections.front().sections_count;
    merged_result.filename = sections.front().filename;

    TextAnalysisResult& total = merged_result.text_analysis_result;

    total.word_count = 0;
    for (const auto& r : sections)
    {
        total.word_count += r.text_analysis_result.word_count;
    }

    std::unordered_map<std::string, std::size_t> global_freq;
    for (const auto& r : sections)
    {
        for (const auto& wf : r.text_analysis_result.top_words)
        {
            global_freq[wf.word] += wf.count;
        }
    }

    std::vector<WordFreq> all_words;
    all_words.reserve(global_freq.size());
    for (const auto& [word, cnt] : global_freq)
    {
        all_words.push_back({word, cnt});
    }

    std::size_t top_n = 0;
    if (!sections.front().text_analysis_result.top_words.empty())
    {
        top_n = sections.front().text_analysis_result.top_words.size();
    }

    if (top_n == 0 || top_n >= all_words.size())
    {
        std::sort(all_words.begin(), all_words.end(),
                  [](const WordFreq& a, const WordFreq& b) 
                  {
                      if (a.count != b.count)
                          return a.count > b.count;
                      return a.word < b.word;
                  });
        total.top_words = std::move(all_words);
    }
    else
    {
        std::nth_element(all_words.begin(),
                         all_words.begin() + top_n,
                         all_words.end(),
                         [](const WordFreq& a, const WordFreq& b) {
                             return a.count > b.count;
                         });

        all_words.resize(top_n);
        std::sort(all_words.begin(), all_words.end(),
                  [](const WordFreq& a, const WordFreq& b) 
                  {
                      if (a.count != b.count)
                          return a.count > b.count;
                      return a.word < b.word;
                  });
        total.top_words = std::move(all_words);
    }

    SentimentStats stats;
    for (const auto& r : sections)
    {
        const auto& s = r.text_analysis_result.sentiment_stats;
        stats.positive_count += s.positive_count;
        stats.negative_count += s.negative_count;
    }
    stats.score = static_cast<int>(stats.positive_count)
                - static_cast<int>(stats.negative_count);

    if (stats.score > 0)
        stats.label = SentimentLabel::Positive;
    else if (stats.score < 0)
        stats.label = SentimentLabel::Negative;
    else
        stats.label = SentimentLabel::Neutral;

    total.sentiment_stats = stats;

    merged_result.data.clear();
    total.modified_text.clear();
    for (const auto& r : sections)
    {
        merged_result.data += r.data; 
        total.modified_text += r.text_analysis_result.modified_text;
    }

    struct HeapNode
    {
        std::size_t len;
        std::size_t src_idx; 
        std::size_t pos;     
    };

    auto cmp = [](const std::string& left, const std::string& right){
        if (left.size() != right.size()) {
            return left.size() < right.size();
        }
        return left <= right;
    };

    std::priority_queue<std::string, std::vector<std::string>, decltype(cmp)> heap(cmp);
    for (std::size_t i = 0; i < sections.size(); ++i)
    {
        const auto& vec = sections[i].text_analysis_result.sorted_sentences;
        for (const auto& s : vec)
        {
            heap.push(s);
        }
    }

    std::vector<std::string> merged_sentences;
    while (!heap.empty())
    {
        std::string node = heap.top();
        heap.pop();

        merged_sentences.push_back(node);
    }

    total.sorted_sentences = std::move(merged_sentences);

    return merged_result;
}

json_type Result::to_statistic_json(const Result& result)
{
    json_type j_result;
    j_result["id"] = result.id;
    j_result["section_id"] = result.section_id;
    j_result["sections_count"] = result.sections_count;
    j_result["file"] = result.filename;
    j_result["stats"] = TextAnalysisResult::to_json(result.text_analysis_result);

    return j_result;
}

json_type Result::to_json(const Result& result)
{
    json_type j_result = to_statistic_json(result);
    j_result["data"] = result.data;
    return j_result;
}

Result Result::from_json(const nlohmann::json& j_result)
{
    Result result;
    result.id = j_result["id"];
    result.section_id = j_result["section_id"];
    result.sections_count = j_result["sections_count"];
    result.filename = j_result["file"];

    result.text_analysis_result = TextAnalysisResult::from_json(j_result["stats"]);

    return result;
}


json_type Metric::to_json(const Metric& metric)
{
    json_type j_metric;

    j_metric["id"] = metric.id;
    j_metric["tag"] = metric.tag == Tag::START ? "START" : "END";
    j_metric["time"] = metric.elapsed_time.count();

    return j_metric;
}

Metric Metric::from_json(const json_type& j_metric)
{
    int id = j_metric["id"];

    Tag tag;
    if (j_metric["tag"] == "START")
    {
        tag = Tag::START;
    }
    else
    {
        tag = Tag::END;
    }
    int time = j_metric["time"];


    return Metric{id, tag, ms{time}};
}


} // namespace tp