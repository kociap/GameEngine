#include "math/vector4.hpp"

#include "math/math.hpp"
#include <cmath>

Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

float& Vector4::component(int index) {
    return (&x)[index];
}

float const& Vector4::component(int index) const {
    return (&x)[index];
}

Vector4& Vector4::operator*=(float a) {
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return *this;
}

bool Vector4::is_zero() const {
    return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
}

float Vector4::length_squared() const {
    return x * x + y * y + z * z + w * w;
}

float Vector4::length() const {
    return std::sqrt(length_squared());
}

Vector4& Vector4::normalize() {
    if (!is_zero()) {
        float inverse_vec_length = math::inv_sqrt(length_squared());
        *this *= inverse_vec_length;
    }

    return *this;
}

Vector4 normalize(Vector4 vec) {
    vec.normalize();
    return vec;
}