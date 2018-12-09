#include "math/vector2.hpp"

const Vector2 Vector2::zero = Vector2(0.0f, 0.0f);
const Vector2 Vector2::one = Vector2(1.0f, 1.0f);
const Vector2 Vector2::up = Vector2(0.0f, 1.0f);
const Vector2 Vector2::right = Vector2(1.0f, 0.0f);

Vector2::Vector2() : x(0), y(0) {}
Vector2::Vector2(float x, float y) : x(x), y(y) {}

float Vector2::dot(Vector2 const& vec1, Vector2 const& vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

Vector2& Vector2::operator-() {
    x = -x;
    y = -y;
    return *this;
} 

bool Vector2::is_zero() const {
    return x == 0.0f && y == 0.0f;
}

float Vector2::length_squared() const {
    return x * x + y * y;
}

float Vector2::length() const {
    return sqrt(x * x + y * y);
}

Vector2& Vector2::normalize() {
    if (!is_zero()) {
        float invertedVecLength = inv_sqrt(x * x + y * y);
        x *= invertedVecLength;
        y *= invertedVecLength;
    }
    return *this;
}

Vector2 Vector2::normalized() const {
    if (is_zero()) {
        float invertedVecLength = inv_sqrt(x * x + y * y);
        return {x * invertedVecLength, y * invertedVecLength};
    } else {
        return {0.0f, 0.0f};
    }
}

Vector2& Vector2::scale(float s) {
    x *= s;
    y *= s;
    return *this;
}

Vector2 operator+(Vector2 const& vec1, Vector2 const& vec2) {
    return {vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector2 operator-(Vector2 const& vec1, Vector2 const& vec2) {
    return {vec1.x - vec2.x, vec1.y - vec2.y};
}