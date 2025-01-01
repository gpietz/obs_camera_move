#include "message_command.h"
#include <sstream>

namespace ObsCamMove {
    static const std::regex COMMAND_REGEX(R"(^(\w+)\(([^)]*)\);?$)");

    MessageCommand::MessageCommand(const String& message) : raw_msg_(message) {
        std::smatch match;

        if (std::regex_match(raw_msg_, match, COMMAND_REGEX)) {
            const String command = match[1];
            const String params = match[2];

            std::istringstream param_stream(command);
            String param;
            while (std::getline(param_stream, param, ',')) {
                params_.push_back(param);
            }
        }
    }

    const String& MessageCommand::get_message() const {
        return raw_msg_;
    }

    const String& MessageCommand::get_command() const {
        return command_;
    }

    const std::vector<String>& MessageCommand::get_params() const {
        return params_;
    }
}
