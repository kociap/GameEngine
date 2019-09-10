#include <math/vector4.hpp>

#include <anton_stl/utility.hpp>
#include <cmath>
#include <math/math.hpp>
#include <math/vector2.hpp>
#include <math/vector3.hpp>

namespace anton_engine {
    Vector4::Vector4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}
    Vector4::Vector4(Vector2 const& vec, float z /* = 0 */, float w /* = 0 */): x(vec.x), y(vec.y), z(z), w(w) {}
    Vector4::Vector4(Vector3 const& vec, float w /* = 0 */): x(vec.x), y(vec.y), z(vec.z), w(w) {}

    float& Vector4::component(int index) {
        return (&x)[index];
    }

    float Vector4::component(int index) const {
        return (&x)[index];
    }

    float& Vector4::operator[](int index) {
        return (&x)[index];
    }

    float Vector4::operator[](int index) const {
        return (&x)[index];
    }

    Vector4& Vector4::operator-() {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
        return *this;
    }

    Vector4 Vector4::operator-() const {
        return {-x, -y, -z, -w};
    }

    Vector4& Vector4::operator+=(Vector4 a) {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
    }

    Vector4& Vector4::operator-=(Vector4 a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
    }

    Vector4& Vector4::operator+=(float a) {
        x += a;
        y += a;
        z += a;
        w += a;
        return *this;
    }

    Vector4& Vector4::operator-=(float a) {
        x -= a;
        y -= a;
        z -= a;
        w -= a;
        return *this;
    }

    Vector4& Vector4::operator*=(float a) {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
        return *this;
    }

    Vector4& Vector4::operator/=(float a) {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
        return *this;
    }

    bool Vector4::is_zero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
    }

    bool Vector4::is_almost_zero(float tolerance) const {
        return math::abs(x) <= tolerance && math::abs(y) <= tolerance && math::abs(z) <= tolerance && math::abs(w) <= tolerance;
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

    Vector4& Vector4::multiply_componentwise(Vector4 const& a) {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        w *= a.w;
        return *this;
    }

    Vector4 operator+(Vector4 a, Vector4 b) {
        a += b;
        return a;
    }

    Vector4 operator-(Vector4 a, Vector4 b) {
        a -= b;
        return a;
    }

    Vector4 operator+(Vector4 a, float b) {
        a += b;
        return a;
    }

    Vector4 operator-(Vector4 a, float b) {
        a -= b;
        return a;
    }

    Vector4 operator*(Vector4 a, float b) {
        a *= b;
        return a;
    }

    Vector4 operator*(float b, Vector4 a) {
        a *= b;
        return a;
    }

    Vector4 operator/(Vector4 a, float b) {
        a /= b;
        return a;
    }

    bool operator==(Vector4 a, Vector4 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    bool operator!=(Vector4 a, Vector4 b) {
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
    }

    void swap(Vector4& a, Vector4& b) {
        anton_stl::swap(a.x, b.x);
        anton_stl::swap(a.y, b.y);
        anton_stl::swap(a.z, b.z);
        anton_stl::swap(a.w, b.w);
    }
} // namespace anton_engine

namespace anton_engine::math {
    float dot(Vector4 a, Vector4 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Vector4 normalize(Vector4 vec) {
        vec.normalize();
        return vec;
    }

    float length(Vector4 v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    Vector4 multiply_componentwise(Vector4 const& a, Vector4 const& b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    }
} // namespace anton_engine::math
