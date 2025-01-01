#pragma once

#include "prerequisites.h"

namespace ObsCamMove {
    [[nodiscard]] bool isNullOrWhitespace(const String& str);
    [[nodiscard]] bool isNullOrWhitespace(const char* str);
    [[nodiscard]] bool isNullOrWhitespace(StringView view);
    [[nodiscard]] String toUpperCase(const String& text);
    [[nodiscard]] String padLeft(const String& input, size_t totalWidth, char paddingChar = ' ');
    [[nodiscard]] String padRight(const String& input, size_t totalWidth, char paddingChar = ' ');
    [[nodiscard]] String trim(const String& str);
    [[nodiscard]] String join_strings(
        const std::vector<String>& strings,
        const String& delimiter = ", ",
        const std::function<const String(String)> &modifier = nullptr,
        bool allow_empty_strings = false);
    [[nodiscard]] String join_strings(
        const std::vector<String>& strings,
        const std::function<const String(String)> &modifier = nullptr,
        bool allow_empty_strings = false);
    [[nodiscard]] String remove_quotes(const String& input, bool with_trim = false);
}
