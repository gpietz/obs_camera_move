#pragma once

#include "prerequisites.h"

namespace ObsCamMove {
    [[nodiscard]] String get_env_var(const String& var_name) noexcept;
    [[nodiscard]] int get_env_var_int(const String& var_name, int default_value = 0) noexcept;
}
