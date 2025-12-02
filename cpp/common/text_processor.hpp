#pragma once

// std
#include <string>
#include <vector>
#include <unordered_map>

// ...
#include "message.hpp" 

namespace tp {
    
class TextProcessor
{
public:
    TextProcessor(Task task
                , std::vector<std::string> positiv_words
                , std::vector<std::string> negative_words);

    Result ProcessText();

private:
    std::size_t CountWords(std::string_view text) const;

    std::unordered_map<std::string, std::size_t> BuildWordFreq(std::string_view text) const;

    std::vector<WordFreq> TopNWords(const std::unordered_map<std::string, std::size_t>& freq, std::size_t top_n) const;

    SentimentStats ComputeSentiment(std::string_view text) const;

    std::string MaskNames(std::string_view text, std::string mask) const;

    std::vector<std::string> SortSentencesByLength(std::string text) const;

private:
    static bool IsWordChar(unsigned char c);
    static bool IsSentenceEnd(unsigned char c);

private:
    Task task_;
    std::vector<std::string> positive_words_;
    std::vector<std::string> negative_words_;

    std::unordered_map<std::string, bool> positive_table_;
    std::unordered_map<std::string, bool> negative_table_;
};

} // namespace tp
