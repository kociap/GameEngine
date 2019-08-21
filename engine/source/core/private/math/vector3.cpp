#include <math/vector3.hpp>

#include <anton_stl/utility.hpp>
#include <cmath>
#include <math/math.hpp>
#include <math/vector2.hpp>
#include <math/vector4.hpp>

Vector3 const Vector3::zero = Vector3(0, 0, 0);
Vector3 const Vector3::one = Vector3(1, 1, 1);
Vector3 const Vector3::up = Vector3(0, 1, 0);
Vector3 const Vector3::forward = Vector3(0, 0, -1);
Vector3 const Vector3::right = Vector3(1, 0, 0);

Vector3::Vector3(float x, float y, float z): x(x), y(y), z(z) {}
Vector3::Vector3(Vector2 const& vec, float z /* = 0 */): x(vec.x), y(vec.y), z(z) {}
Vector3::Vector3(Vector4 const& vec): x(vec.x), y(vec.y), z(vec.z) {}

float& Vector3::component(int index) {
    return (&x)[index];
}

float Vector3::component(int index) const {
    return (&x)[index];
}

float& Vector3::operator[](int index) {
    return (&x)[index];
}

float Vector3::operator[](int index) const {
    return (&x)[index];
}

Vector3& Vector3::operator-() {
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

Vector3 Vector3::operator-() const {
    return {-x, -y, -z};
}

Vector3& Vector3::operator+=(Vector3 a) {
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
}

Vector3& Vector3::operator-=(Vector3 a) {
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
}

Vector3& Vector3::operator+=(float a) {
    x += a;
    y += a;
    z += a;
    return *this;
}

Vector3& Vector3::operator-=(float a) {
    x -= a;
    y -= a;
    z -= a;
    return *this;
}

Vector3& Vector3::operator*=(float a) {
    x *= a;
    y *= a;
    z *= a;
    return *this;
}

Vector3& Vector3::operator/=(float a) {
    x /= a;
    y /= a;
    z /= a;
    return *this;
}

bool Vector3::is_zero() const {
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

bool Vector3::is_almost_zero(float tolerance) const {
    return math::abs(x) <= tolerance && math::abs(y) <= tolerance && math::abs(z) <= tolerance;
}

float Vector3::length_squared() const {
    return x * x + y * y + z * z;
}

float Vector3::length() const {
    return std::sqrt(length_squared());
}

Vector3& Vector3::normalize() {
    if (!is_zero()) {
        float inverse_vec_length = math::inv_sqrt(length_squared());
        *this *= inverse_vec_length;
    }
    return *this;
}

Vector3& Vector3::multiply_componentwise(Vector3 const& a) {
    x *= a.x;
    y *= a.y;
    z *= a.z;
    return *this;
}

Vector3 operator+(Vector3 a, Vector3 b) {
    a += b;
    return a;
}

Vector3 operator-(Vector3 a, Vector3 b) {
    a -= b;
    return a;
}

Vector3 operator+(Vector3 a, float b) {
    a += b;
    return a;
}

Vector3 operator-(Vector3 a, float b) {
    a -= b;
    return a;
}

Vector3 operator*(Vector3 a, float b) {
    a *= b;
    return a;
}

Vector3 operator*(float b, Vector3 a) {
    a *= b;
    return a;
}

Vector3 operator/(Vector3 a, float b) {
    a /= b;
    return a;
}

bool operator==(Vector3 a, Vector3 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(Vector3 a, Vector3 b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

void swap(Vector3& a, Vector3& b) {
    anton_stl::swap(a.x, b.x);
    anton_stl::swap(a.y, b.y);
    anton_stl::swap(a.z, b.z);
}

namespace math {
    float dot(Vector3 const& vec1, Vector3 const& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    Vector3 cross(Vector3 const& vec1, Vector3 const& vec2) {
        return Vector3(vec1.y * vec2.z - vec2.y * vec1.z, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x);
    }

    Vector3 normalize(Vector3 vec) {
        vec.normalize();
        return vec;
    }

    float length(Vector3 v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    Vector3 multiply_componentwise(Vector3 const& a, Vector3 const& b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z};
    }
} // namespace math
