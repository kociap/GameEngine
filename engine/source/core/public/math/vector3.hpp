#ifndef GAME_ENGINE_CORE_MATH_VECTOR3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_VECTOR3_HPP_INCLUDE

#include "math.hpp"

class Vector3 {
public:
    float x = 0;
    float y = 0;
    float z = 0;

    Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
    /*explicit Vector3(Vector2 const& vec) : x(vec.x), y(vec.y), z(0) {}
    explicit Vector3(Vector2&& vec) : x(vec.x), y(vec.y), z(0) {}
    explicit Vector3(Vector4 const& vec) : x(vec.x), y(vec.y), z(vec.z) {}
    explicit Vector3(Vector4&& vec) : x(vec.x), y(vec.y), z(vec.z) {}*/

    static float dot(Vector3 const& vec1, Vector3 const& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    static Vector3 cross(Vector3 const& vec1, Vector3 const& vec2) {}

    static Vector3 cross_direction(Vector3 const& vec1, Vector3 const& vec2) {}

    Vector3 operator+(Vector3 const& vec) {
        return {x + vec.x, y + vec.y, z + vec.z};
    }

    Vector3 operator-(Vector3 const& vec) {
        return {x - vec.x, y - vec.y, z - vec.z};
    }

    Vector3& operator-() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    // Requires correct comparison
    bool is_zero() const {
        return truncate_float(x) == 0.0f && truncate_float(y) == 0.0f && truncate_float(z) == 0.0f;
    }

    float length_squared() const {
        return x * x + y * y + z * z;
    }

    float length() {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3& normalize() {
        float invertedVecLength = inv_sqrt(x * x + y * y + z * z);
        x *= invertedVecLength;
        y *= invertedVecLength;
        z *= invertedVecLength;
        return *this;
    }

    Vector3 normalized() const {
        float invertedVecLength = inv_sqrt(x * x + y * y + z * z);
        return {x * invertedVecLength, y * invertedVecLength, z * invertedVecLength};
    }

    Vector3& scale(float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
};

#endif // !GAME_ENGINE_CORE_MATH_VECTOR3_HPP_INCLUDE