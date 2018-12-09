#ifndef GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE

#include "math.hpp"
#include <math.h>

class Vector4 {
public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

	Vector4(float x, float y, float z, float w);

    // Requires correct comparison
    bool is_zero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
    }
};

#endif // !GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE
