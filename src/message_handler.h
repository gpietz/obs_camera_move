#pragma once

#include "prerequisites.h"
#include "message_command.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>

namespace ObsCamMove {
    class MessageHandler {
    public:
        using HandlerFunction = std::function<String(const MessageCommand&)>;

        MessageHandler();

        std::optional<std::string> process_message(const std::string& message);

    private:
        std::unordered_map<std::string, HandlerFunction> handlers_;

        void register_handler(const std::string& command, HandlerFunction handler);

        static String log_error(const String& message);
        static String handle_move_to(const MessageCommand& command);
        static String handle_move_by(const MessageCommand& command);
    };
}
