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

    float max(float a, float b) {
        return a > b ? a : b;
    }

    float min(float a, float b) {
        return a < b ? a : b;
    }

	float abs(float a) {
        return a >= 0 ? a : -a;
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