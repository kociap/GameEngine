#include <math/math.hpp>

#include <cmath>

namespace math {
    float inv_sqrt(float a) {
        return 1 / sqrt(a);
    }

    float sign(float a) {
        return static_cast<float>((a > 0.0f) - (a < 0.0f));
    }

    float sin(float angle) {
        return std::sinf(angle);
    }

    float cos(float angle) {
        return std::cosf(angle);
    }

    float abs(float a) {
        return std::abs(a);
    }

    float step_to_value(float current, float target, float change) {
        float delta = target - current;
        if (abs(delta) > change) {
            return current + sign(delta) * change;
        } else {
            return target;
        }
    }
} // namespace math
