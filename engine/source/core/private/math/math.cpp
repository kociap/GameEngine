#include "math/math.hpp"

#include <cmath>

namespace math {
    float inv_sqrt(float a) {
        return 1 / sqrt(a);
    }

    float sign(float a) {
        if (a > 0.0f) {
            return 1.0f;
        } else if (a < 0.0f) {
            return -1.0f;
        } else {
            return 0.0f;
        }
    }
} // namespace math