#pragma once

#include "prerequisites.h"

namespace ObsCamMove {
    [[nodiscard]] constexpr bool isNullOrWhitespace(const char* str);
    [[nodiscard]] constexpr bool isNullOrWhitespace(StringView view);
    [[nodiscard]] String toUpperCase(const String& text);
    [[nodiscard]] String padLeft(const String& input, size_t totalWidth, char paddingChar = ' ');
    [[nodiscard]] String padRight(const String& input, size_t totalWidth, char paddingChar = ' ');
    [[nodiscard]] String trim(const String& str);
}
