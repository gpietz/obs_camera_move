#include "message_command.h"
#include "logger.h"
#include "string_utils.h"
#include <sstream>

namespace ObsCamMove {
    static const std::regex COMMAND_REGEX(R"(^(\w+)\(([^)]*)\);?$)");

    MessageCommand::MessageCommand(const String& message) : raw_msg_(message) {
        log(LogLevel::DEBUG, "Parsing message command: " + message);
        if (std::smatch match; std::regex_match(raw_msg_, match, COMMAND_REGEX)) {
            command_ = match[1];
            const String params = match[2];

            // Extract command parameters
            std::istringstream param_stream(params);
            String param;
            while (std::getline(param_stream, param, ',')) {
                params_.push_back(trim(param));
            }

            log(LogLevel::DEBUG, "Command parse: " + command_);
            log(LogLevel::DEBUG, "Parameters parsed: " + std::to_string(params_.size()));
        } else {
            log(LogLevel::ERROR, "Invalid message command: " + raw_msg_);
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
