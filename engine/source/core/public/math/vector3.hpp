#ifndef CORE_MATH_VECTOR3_HPP_INCLUDE
#define CORE_MATH_VECTOR3_HPP_INCLUDE

#include "math.hpp"

class Vector3 {
public:
    float x = 0;
    float y = 0;
    float z = 0;

    Vector3(float x, float y, float z);

    float& component(int);
    float const& component(int) const;

    static float dot(Vector3 const& vec1, Vector3 const& vec2);

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
        return x == 0.0f && y == 0.0f && z == 0.0f;
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

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE