#include "message_handler.h"
#include "message_command.h"
#include "logger.h"
#include <regex>

#include "camera_controller.h"

namespace ObsCamMove {
    MessageHandler::MessageHandler() {
        register_handler("move_to", handle_move_to);
        register_handler("move_by", handle_move_by);
    }

    void MessageHandler::register_handler(const std::string& command, HandlerFunction handler) {
        handlers_[command] = std::move(handler);
    }

    std::optional<std::string> MessageHandler::process_message(const std::string& message) {
        try {
            const MessageCommand message_command(message);

            if (const auto it = handlers_.find(message_command.get_command()); it != handlers_.end()) {
                return it->second(message_command);
            }

            log(LogLevel::ERROR, String("Unknown command: ") + message_command.get_command());
            return std::nullopt;
        } catch (const std::exception& e) {
            log(LogLevel::ERROR, String("Error processing message: ") + e.what());
            return std::nullopt;
        }
    }

    String MessageHandler::log_error(const String& message) {
        log(LogLevel::ERROR, message);
        return String("ERROR: ") + message;
    }

    String MessageHandler::handle_move_to(const MessageCommand& command) {
        const auto& params = command.get_params();

        if (params.size() != 3) {
            return log_error("Wrong number of parameters for move_to command: ") + std::to_string(params.size());
        }

        try {
            const int x = std::stoi(String(params[0]));
            const int y = std::stoi(String(params[1]));
            const int duration = std::stoi(String(params[2]));
            CameraController::getInstance().move_to(x, y, duration);
            return "OK";
        } catch (const std::invalid_argument& e) {
            return log_error("Invalid parameter(s) for move_to. All parameters must be integers.");
        } catch (const std::out_of_range& e) {
            return log_error("Parameter(s) out of range for move_to.");
        }
    }

    String MessageHandler::handle_move_by(const MessageCommand& command) {
        const auto& params = command.get_params();

        if (params.size() != 3) {
            return log_error("Wrong number of parameters for move_to command: ") + std::to_string(params.size());
        }

        try {
            const int dx = std::stoi(String(params[0]));
            const int dy = std::stoi(String(params[1]));
            const int duration = std::stoi(String(params[2]));
            CameraController::getInstance().move_by(dx, dy, duration);
            return "OK";
        } catch (const std::invalid_argument& e) {
            return log_error("Invalid parameter(s) for move_to. All parameters must be integers.");
        } catch (const std::out_of_range& e) {
            return log_error("Parameter(s) out of range for move_to.");
        }
    }
}
