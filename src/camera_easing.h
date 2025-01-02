#pragma once

#include "prerequisites.h"
#include <cmath>

namespace ObsCamMove {
    enum class CameraEasingType : uint8_t {
        Linear           = 0,
        SmoothStep       = 1,
        EaseInQuad       = 2,
        EaseOutQuad      = 3,
        EaseInOutQuad    = 4,
        EaseInQuint      = 5,
        EaseOutQuint     = 6,
        EaseInOutQuint   = 7,
        EaseInElastic    = 8,
        EaseOutElastic   = 9,
        EaseInOutElastic = 10,
    };

    class CameraEasing {
    public:
        static CameraEasingType to_camera_easing_type(const u8 easing_type_value) {
            if (easing_type_value <= static_cast<u8>(CameraEasingType::EaseInOutElastic)) {
                return static_cast<CameraEasingType>(easing_type_value);
            }
            throw std::invalid_argument("Invalid camera easing type value: " + std::to_string(easing_type_value));
        }

        static constexpr float calculate(const CameraEasingType type, const float t) {
            switch (type) {
                case CameraEasingType::SmoothStep:
                    return t * t * (3 - 2 * t);
                case CameraEasingType::EaseInQuad:
                    return t * t;
                case CameraEasingType::EaseOutQuad:
                    return t * (2 - t);
                case CameraEasingType::EaseInOutQuad:
                    return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
                case CameraEasingType::EaseInQuint:
                    return t * t * t * t * t;
                case CameraEasingType::EaseOutQuint:
                    return 1 - std::pow(1 - t,5);
                case CameraEasingType::EaseInOutQuint:
                    return t < 0.5 ? 16 * t * t * t * t * t : 1 - std::pow(-2 * t + 2, 5) / 2;
                case CameraEasingType::EaseInElastic:
                    return ease_in_elastic(t);
                case CameraEasingType::EaseOutElastic:
                    return ease_out_elastic(t);
                case CameraEasingType::EaseInOutElastic:
                    return ease_in_out_elastic(t);
                default:
                    return t;
            }
        }

    private:
        static float ease_in_elastic(const float t) {
            constexpr auto c4 = (2 * M_PI) / 3;

            if (t == 0 || t == 1) return t;
            return -std::pow(2, 10 * t - 10) * std::sin((t * 10 - 10.75) * c4);
        }

        static float ease_out_elastic(const float t) {
            constexpr auto c4 = (2 * M_PI) / 3;

            if (t == 0 || t == 1) return t;
            return std::pow(2, -10 * t) * std::sin((t * 10 - 0.75) * c4) + 1;
        }

        static float ease_in_out_elastic(const float t) {
            constexpr double c5 = (2 * M_PI) / 4.5;

            if (t == 0 || t == 1) return t;
            return (t < 0.5)
                ? -(std::pow(2, 20 * t - 10) * std::sin((20 * t - 11.125) * c5)) / 2
                : (std::pow(2, -20 * t + 10) * std::sin((20 * t - 11.125) * c5)) / 2 + 1;
        }
    };
}
