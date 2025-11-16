#pragma once

// std
#include <string>
#include <string_view>
#include <vector>

namespace tp {

int count_words(const std::string_view& text_view);
std::vector<std::pair<std::string, int>> top_words(const std::string_view& text_view, int top_length);

} // namespace tp
