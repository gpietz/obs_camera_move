#pragma once

#include "prerequisites.h"
#include <regex>

namespace ObsCamMove {
    class MessageCommand {
    public:
        explicit MessageCommand(const String& message);

        [[nodiscard]] const String& get_message() const;
        [[nodiscard]] const String& get_command() const;
        [[nodiscard]] const std::vector<String>& get_params() const;

    private:
        const String raw_msg_;
        String command_;
        std::vector<String> params_;
    };
}
