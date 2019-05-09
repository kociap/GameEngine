#include "math/vector2.hpp"

#include "math/vector3.hpp"
#include "math/vector4.hpp"

#include "math/math.hpp"
#include <cmath>
#include <utility>

const Vector2 Vector2::zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::one = Vector2(1.0f, 1.0f);
const Vector2 Vector2::up = Vector2(0.0f, 1.0f);
const Vector2 Vector2::right = Vector2(1.0f, 0.0f);

Vector2::Vector2(float x, float y): x(x), y(y) {}
Vector2::Vector2(Vector3 const& vec): x(vec.x), y(vec.y) {}
Vector2::Vector2(Vector4 const& vec): x(vec.x), y(vec.y) {}

float& Vector2::component(int index) {
    return (&x)[index];
}

float Vector2::component(int index) const {
    return (&x)[index];
}

Vector2& Vector2::operator-() {
    x = -x;
    y = -y;
    return *this;
}

Vector2 Vector2::operator-() const {
    return {-x, -y};
}

Vector2& Vector2::operator+=(Vector2 const& a) {
    x += a.x;
    y += a.y;
    return *this;
}

Vector2& Vector2::operator-=(Vector2 const& a) {
    x += a.x;
    y += a.y;
    return *this;
}

Vector2& Vector2::operator*=(float a) {
    x *= a;
    y *= a;
    return *this;
}

bool Vector2::is_zero() const {
    return x == 0.0f && y == 0.0f;
}

bool Vector2::is_almost_zero(float tolerance) const {
    return math::abs(x) <= tolerance && math::abs(y) <= tolerance;
}

float Vector2::length_squared() const {
    return x * x + y * y;
}

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

Vector2& Vector2::normalize() {
    if (!is_zero()) {
        float inverse_vec_length = math::inv_sqrt(length_squared());
        *this *= inverse_vec_length;
    }
    return *this;
}

Vector2& Vector2::multiply_componentwise(Vector2 const& a) {
    x *= a.x;
    y *= a.y;
    return *this;
}

Vector2 operator+(Vector2 const& vec1, Vector2 const& vec2) {
    return {vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector2 operator-(Vector2 const& vec1, Vector2 const& vec2) {
    return {vec1.x - vec2.x, vec1.y - vec2.y};
}

Vector2 operator*(Vector2 const& a, float b) {
    return {a.x * b, a.y * b};
}

Vector2 operator*(float b, Vector2 const& a) {
    return {a.x * b, a.y * b};
}

Vector2 operator/(Vector2 const& a, float b) {
    return {a.x / b, a.y / b};
}

void swap(Vector2& a, Vector2& b) {
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
}

bool operator==(Vector2 a, Vector2 b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(Vector2 a, Vector2 b) {
    return a.x != b.x || a.y != b.y;
}

namespace math {
    float dot(Vector2 const& vec1, Vector2 const& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y;
    }

    Vector2 normalize(Vector2 vec) {
        vec.normalize();
        return vec;
    }

    Vector2 multiply_componentwise(Vector2 const& a, Vector2 const& b) {
        return {a.x * b.x, a.y * b.y};
    }
} // namespace math