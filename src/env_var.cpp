#include "env_var.h"

namespace ObsCamMove {
    String get_env_var(const String& var_name) noexcept {
        try {
            const char* value = std::getenv(var_name.c_str());
            if (!value) {
                return "";
            }
            return String(value);
        } catch (std::exception&) {
            return "";
        }
    }

    int get_env_var_int(const String& var_name, const int default_value) noexcept {
        try {
            const char* value = std::getenv(var_name.c_str());
            if (!value) {
                return  default_value;
            }
            return std::stoi(String(value));
        } catch (const std::exception&) {
            return default_value;
        }
    }
}
