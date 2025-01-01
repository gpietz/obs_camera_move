#pragma once

#include "prerequisites.h"
#include <fstream>
#include <mutex>
#include <string>

namespace ObsCamMove {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    class Logger {
    public:
        static Logger& get_instance() {
            static Logger instance;
            return instance;
        }

        void set_log_file(const String& log_path);
        void log(LogLevel level, const std::string& message);
        String get_log_file_path();

    private:
        String log_file_path_;
        std::string logFilePath_;
        std::mutex mutex_;

        Logger();
        Logger(Logger const&) = delete;
        Logger& operator=(Logger const&) = delete;

        void write_to_logfile(LogLevel level, const std::string& message);

        [[nodiscard]] static String get_log_level_string(LogLevel level);
        [[nodiscard]] static String get_default_log_file_path();

        bool ensure_log_path_exists();
    };

    inline void log(const LogLevel level, const std::string& message) {
        Logger::get_instance().log(level, message);
    }
}
