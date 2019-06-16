#ifndef CORE_MATH_MATH_HPP_INCLUDE
#define CORE_MATH_MATH_HPP_INCLUDE

#include <limits>

// Safety measures
#undef max
#undef min

namespace math {
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

    float inv_sqrt(float number);
    float sign(float);
    float abs(float);
    float sin(float);
    float cos(float);
    float step_to_value(float current, float target, float change);

    template <typename T>
    T max(T a, T b) {
        return a > b ? a : b;
    }

    template <typename T>
    T min(T a, T b) {
        return a < b ? a : b;
    }
} // namespace math

#endif // !CORE_MATH_MATH_HPP_INCLUDE
