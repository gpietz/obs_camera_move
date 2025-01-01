#include "string_utils.h"
#include <algorithm>
#include <locale>

namespace ObsCamMove {
    bool isNullOrWhitespace(const String& str) {
        return isNullOrWhitespace(str.c_str());
    }

    bool isNullOrWhitespace(const char* str) {
        return str == nullptr || isNullOrWhitespace(StringView(str));
    }

    bool isNullOrWhitespace(StringView view) {
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

    String join_strings(
        const std::vector<String>& strings,
        const String& delimiter,
        const std::function<const String(String)> &modifier,
        const bool allow_empty_strings) {

        std::ostringstream oss;
        bool first = true;

        for (const auto& str : strings) {
            // ReSharper disable once CppTooWideScopeInitStatement
            const String current = modifier ? modifier(str) : str;
            if (!current.empty() || allow_empty_strings) {
                if (!first && !delimiter.empty()) {
                    oss << delimiter;
                }
                oss << current;
                first = false;
            }
        }

        return oss.str();
    }

    String join_strings(
        const std::vector<String>& strings,
        const std::function<const String(String)> &modifier,
        const bool allow_empty_strings) {
        return join_strings(strings, ", ", modifier, allow_empty_strings);
    }

    String remove_quotes(const String& input, const bool with_trim) {
        if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
            const auto unquoted = input.substr(1, input.size() - 2);
            return with_trim ? trim(unquoted) : unquoted;
        }
        return input;
    }
}
