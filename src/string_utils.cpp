#include "string_utils.h"
#include <algorithm>
#include <locale>

namespace ObsCamMove {
    constexpr bool isNullOrWhitespace(const char* str) {
        return str == nullptr || isNullOrWhitespace(StringView(str));
    }

    constexpr bool isNullOrWhitespace(StringView view) {
        return view.empty() || std::ranges::all_of(view, [](unsigned char c) {
           return std::isspace(c);
        });
    }

    String toUpperCase(const String& text) {
        std::string result = text;
        std::locale loc("");
        std::ranges::transform(result, result.begin(),[&loc](const unsigned char c) {
            return std::toupper(c, loc);
        });
        return result;
    }

    String padLeft(const String& input, const size_t totalWidth, const char paddingChar) {
        if (input.length() >= totalWidth) {
            return input;
        }
        return std::string(totalWidth - input.length(), paddingChar) + input;
    }

    String padRight(const String& input, const size_t totalWidth, const char paddingChar) {
        if (input.length() >= totalWidth) {
            return input;
        }
        return input + std::string(totalWidth - input.length(), paddingChar);
    }

    String trim(const String& str) {
        const auto start = std::ranges::find_if_not(str,
            [](const unsigned char c) { return std::isspace(c) || c == '\n' || c == '\t'; });
        const auto end = std::find_if_not(str.rbegin(), str.rend(),
            [](const unsigned char c) { return std::isspace(c) || c == '\n' || c == '\t'; }).base();
        return (start < end ? std::string(start, end) : "");
    }
}
