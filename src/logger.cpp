#include "logger.h"
#include "string_utils.h"
#include <obs-module.h>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace ObsCamMove {
    Logger::Logger() {

    }

    void Logger::set_log_file(const String& log_path) {
        log_file_path_ = log_path;

        // Create directory if not exist
        try {
            if (const std::filesystem::path log_dir = std::filesystem::path(log_path).parent_path(); !exists(log_dir)) {
                create_directory(log_dir);
            }
        }
        catch (const std::exception&) {
            log_file_path_ = "";
        }
    }

    void Logger::log(const LogLevel level, const std::string& message) {
        std::lock_guard lock(mutex_);

        // Write to OBS protocol
        switch (level) {
            case LogLevel::DEBUG:
                blog(LOG_DEBUG, "%s", message.c_str());
                break;
            case LogLevel::INFO:
                blog(LOG_INFO, "%s", message.c_str());
                break;
            case LogLevel::WARN:
                blog(LOG_WARNING, "%s", message.c_str());
                break;
            case LogLevel::ERROR:
                blog(LOG_ERROR, "%s", message.c_str());
                break;
            default:
                break;;
        }

        write_to_logfile(level, message);
    }

    void Logger::write_to_logfile(const LogLevel level, const std::string& message) {
        if (!ensure_log_path_exists()) {
            return;
        }

        std::ofstream log_file(log_file_path_, std::ios::out | std::ios::app);
        if (!log_file.is_open()) {
            return;
        }

        const auto now = std::chrono::system_clock::now();
        const auto time_now = std::chrono::system_clock::to_time_t(now);
        const auto tm_now = *std::localtime(&time_now);

        std::ostringstream timestamp;
        timestamp << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S");

        log_file << "[" << get_log_level_string(level) << "] "
                 << timestamp.str() << " | "
                 << message
                 << std::endl;
    }

    String Logger::get_log_level_string(const LogLevel level) {
        constexpr int levelLength = 7;
        switch (level) {
            case LogLevel::DEBUG: return  padRight("DEBUG", levelLength);
            case LogLevel::INFO: return  padRight("INFO", levelLength);
            case LogLevel::WARN: return padRight("WARN", levelLength);
            case LogLevel::ERROR: return padRight("ERROR", levelLength);
            default: return padRight("UNKNOWN", levelLength);
        }
    }

    String Logger::get_default_log_file_path() {
#ifdef _WIN32
        char* buffer = nullptr;
        size_t buffer_size = 0;

        // Get application data path in user directory (secure method)
        if (_dupenv_s(&buffer, &buffer_size, "LOCALAPPDATA") != 0 || buffer == nullptr) {
            throw std::runtime_error("Failed to get LOCALAPPDATA environment variable");
        }

        const String base_path(buffer);
        free(buffer);

        return base_path + "\\ObsCameraMove\\ObsCameraMove.log";
#else
        // Logger is currently implemented only for Windows.
        return "";
#endif
    }

    bool Logger::ensure_log_path_exists() {
        if (!log_file_path_.empty()) {
            return true;
        }

        try {
            set_log_file(get_default_log_file_path());
            return !log_file_path_.empty();
        } catch (std::exception&) {
            return false;
        }
    }
}
