#include "text_processor.hpp"

namespace tp {
    
TextProcessor::TextProcessor(Task task
                , std::vector<std::string> positive_words
                , std::vector<std::string> negative_words)
    : task_{std::move(task)}
    , positive_words_{std::move(positive_words)}
    , negative_words_{std::move(negative_words)}
{
    for (const auto& w : positive_words_) 
    {
        std::string l = w;
        std::transform(l.begin(), l.end(), l.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        positive_table_[std::move(l)] = true;
    }
    for (const auto& w : negative_words_) 
    {
        std::string l = w;
        std::transform(l.begin(), l.end(), l.begin()
        , [](unsigned char c) { return std::tolower(c); });
        negative_table_[std::move(l)] = true;
    }   
}

Result TextProcessor::ProcessText()
{
    TextAnalysisResult text_stats;
    if (task_.option.count_word)
    {
        text_stats.word_count = CountWords(task_.data);
    }

    if (task_.option.top_words)
    {
        auto table = BuildWordFreq(task_.data);
        text_stats.top_words = TopNWords(table, task_.option.top_n);
    }

    if (task_.option.sentiment)
    {
        text_stats.sentiment_stats = ComputeSentiment(task_.data);
    }

    if (task_.option.modify_text)
    {
        // text_stats.modified_text = MaskNames(task_.data, task_.option.mask);
    }

    if (task_.option.sort_sentences)
    {
        text_stats.sorted_sentences = SortSentencesByLength(task_.data);
    }

    std::string text_data = std::move(task_.data);
    Result result{task_, std::move(task_.data), std::move(text_stats)};
    return result;
}

std::size_t TextProcessor::CountWords(std::string_view text) const
{
    std::size_t count = 0;
    bool in_word = false;

    for (unsigned char c : text) 
    {
        if (IsWordChar(c)) 
        {
            if (!in_word) 
            {
                in_word = true;
                ++count;
            }
        } 
        else 
        {
            in_word = false;
        }
    }

    return count;
}

std::unordered_map<std::string, std::size_t> TextProcessor::BuildWordFreq(std::string_view text) const
{
    std::unordered_map<std::string, std::size_t> freq;

    std::size_t i = 0;
    const std::size_t n = text.size();

    while (i < n) 
    {
        while (i < n && !IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            ++i;
        }
        if (i >= n) break;

        std::size_t start = i;
        while (i < n && IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            ++i;
        }
        std::size_t len = i - start;
        if (len == 0) continue;

        std::string word(text.substr(start, len));
        
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        ++freq[word];
    }
    return freq;
}

std::vector<WordFreq> TextProcessor::TopNWords(const std::unordered_map<std::string, std::size_t>& freq, std::size_t top_n) const
{
    std::vector<WordFreq> all;
    all.reserve(freq.size());
    for (const auto& [w, c] : freq) 
    {
        all.push_back({w, c});
    }

    if (top_n >= all.size()) 
    {
        std::sort(all.begin(), all.end(),
                  [](const WordFreq& a, const WordFreq& b) {
                      return a.count > b.count; // descending
                  });
        return all;
    }

    // sorting only first top_n elements
    std::nth_element(all.begin(), all.begin() + top_n, all.end(),
                     [](const WordFreq& a, const WordFreq& b) {
                         return a.count > b.count;
                     });

    all.resize(top_n);
    std::sort(all.begin(), all.end(),
              [](const WordFreq& a, const WordFreq& b) {
                  return a.count > b.count;
              });
    return all;
}

SentimentStats TextProcessor::ComputeSentiment(std::string_view text) const
{
    SentimentStats stats;

    std::size_t i = 0;
    const std::size_t n = text.size();

    while (i < n) 
    {
        while (i < n && !IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            ++i;
        }
        if (i >= n) break;

        std::size_t start = i;
        while (i < n && IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            ++i;
        }
        std::size_t len = i - start;
        if (len == 0) continue;

        std::string word(text.substr(start, len));
        std::transform(word.begin(), word.end(), word.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if (positive_table_.count(word)) 
        {
            ++stats.positive_count;
        }
        else if (negative_table_.count(word)) 
        {
            ++stats.negative_count;
        }
    }

    stats.score = static_cast<int>(stats.positive_count) -
                  static_cast<int>(stats.negative_count);

    if (stats.score > 0) 
    {
        stats.label = SentimentLabel::Positive;
    }
    else if (stats.score < 0) 
    {
        stats.label = SentimentLabel::Negative;
    }
    else 
    {
        stats.label = SentimentLabel::Neutral;
    }

    return stats;
}

std::string TextProcessor::MaskNames(std::string_view text, std::string mask) const
{
    std::string result;
    result.reserve(text.size());

    const std::size_t n = text.size();
    std::size_t i = 0;

    bool in_sentence_start = true;

    while (i < n) {
        if (!IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            unsigned char c = text[i];
            result.push_back(static_cast<char>(c));
            if (IsSentenceEnd(c)) 
            {
                in_sentence_start = true;
            }
            ++i;
            continue;
        }

        std::size_t start = i;
        while (i < n && IsWordChar(static_cast<unsigned char>(text[i]))) 
        {
            ++i;
        }
        std::size_t len = i - start;
        std::string_view word = text.substr(start, len);

        bool is_name = false;
        if (!word.empty()) 
        {
            unsigned char first = static_cast<unsigned char>(word[0]);
            if (std::isalpha(first) && std::isupper(first) && !in_sentence_start) 
            {
                is_name = true;
            }
        }

        if (is_name) 
        {
            result.append(mask);
        }
        else 
        {
            result.append(word.begin(), word.end());
        }

        in_sentence_start = false;
    }

    return result;
}

std::vector<std::string> TextProcessor::SortSentencesByLength(std::string text) const
{
    std::vector<std::string> sentences;

    const std::size_t n = text.size();
    std::size_t start = 0;

    for (std::size_t i = 0; i < n; ++i) 
    {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (IsSentenceEnd(c)) {
            std::size_t end = i + 1;
            std::string raw = text.substr(start, end - start);


            std::size_t left = 0;
            while (left < raw.size() &&
                   std::isspace(static_cast<unsigned char>(raw[left]))) {
                ++left;
            }
            std::size_t right = raw.size();
            while (right > left &&
                   std::isspace(static_cast<unsigned char>(raw[right - 1]))) {
                --right;
            }
            if (right > left) {
                sentences.emplace_back(raw.substr(left, right - left));
            }

            start = end;
        }
    }

    if (start < n) {
        std::string raw = text.substr(start);
        std::size_t left = 0;
        while (left < raw.size() &&
               std::isspace(static_cast<unsigned char>(raw[left]))) {
            ++left;
        }
        std::size_t right = raw.size();
        while (right > left &&
               std::isspace(static_cast<unsigned char>(raw[right - 1]))) {
            --right;
        }
        if (right > left) {
            sentences.emplace_back(raw.substr(left, right - left));
        }
    }

    std::sort(sentences.begin(), sentences.end(),
              [](const std::string& a, const std::string& b) {
                  if (a.size() != b.size()) {
                      return a.size() >= b.size();
                  }
                  return a > b;
              });

    return sentences;
}

bool TextProcessor::IsWordChar(unsigned char c) 
{
    return std::isalnum(c) || c == '\'' || c == '-';
}

bool TextProcessor::IsSentenceEnd(unsigned char c) 
{
    return c == '.' || c == '!' || c == '?';
}

} // namespace tp
