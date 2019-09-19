#ifndef CORE_MATH_MATH_HPP_INCLUDE
#define CORE_MATH_MATH_HPP_INCLUDE

#include <cmath>
#include <limits>

// Safety measures
#undef max
#undef min

namespace anton_engine::math {
    template <typename T>
    struct constants;

    template <>
    struct constants<float> {
        constexpr static float pi = 3.14159265f;
        constexpr static float deg_to_rad = pi / 180.0f;
        constexpr static float rad_to_deg = 180.0f / pi;
        // Smallest number such that 1.0 + epsilon != 1.0
        constexpr static float epsilon = std::numeric_limits<float>::epsilon();
        constexpr static float infinity = std::numeric_limits<float>::infinity();
    };

    template <>
    struct constants<double> {
        constexpr static double pi = 3.141592653589793;
        constexpr static double deg_to_rad = pi / 180.0;
        constexpr static double rad_to_deg = 180.0 / pi;
        // Smallest number such that 1.0 + epsilon != 1.0
        constexpr static double epsilon = std::numeric_limits<double>::epsilon();
        constexpr static double infinity = std::numeric_limits<double>::infinity();
    };

    constexpr float radians(float degrees) {
        return degrees * constants<float>::deg_to_rad;
    }

    constexpr float degrees(float radians) {
        return radians * constants<float>::rad_to_deg;
    }

    inline float inv_sqrt(float a) {
        return 1 / sqrt(a);
    }

    inline float sign(float a) {
        return static_cast<float>((a > 0.0f) - (a < 0.0f));
    }

    inline float sin(float angle) {
        return std::sinf(angle);
    }

    inline float cos(float angle) {
        return std::cosf(angle);
    }

    inline float abs(float a) {
        return std::abs(a);
    }

    inline float step_to_value(float current, float target, float change) {
        float delta = target - current;
        if (abs(delta) > change) {
            return current + sign(delta) * change;
        } else {
            return target;
        }
    }

    template <typename T>
    T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    T min(T a, T b) {
        return a < b ? a : b;
    }
} // namespace anton_engine::math
#endif // !CORE_MATH_MATH_HPP_INCLUDE
