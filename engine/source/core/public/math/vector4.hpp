#ifndef CORE_MATH_VECTOR4_HPP_INCLUDE
#define CORE_MATH_VECTOR4_HPP_INCLUDE

#include <anton_stl/utility.hpp>
#include <cmath>
#include <math/math.hpp>

namespace anton_engine {
    class Vector2;
    class Vector3;

    class Vector4 {
    public:
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;

        Vector4() = default;
        Vector4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}
        explicit Vector4(Vector2 const&, float z = 0, float w = 0);
        explicit Vector4(Vector3 const&, float w = 0);

        float& operator[](int);
        float operator[](int) const;

        Vector4& operator-();
        Vector4 operator-() const;
        Vector4& operator+=(Vector4);
        Vector4& operator-=(Vector4);
        Vector4& operator+=(float);
        Vector4& operator-=(float);
        Vector4& operator*=(float);
        Vector4& operator/=(float);

        // Check if all components are equal 0
        bool is_zero() const;
        bool is_almost_zero(float tolerance = 0.00001f) const;

        float length_squared() const;
        float length() const;

        // If vector is non-zero, normalizes the vector.
        // Otherwise leaves it unchanged
        Vector4& normalize();

        Vector4& multiply_componentwise(Vector4 const&);
    };

    Vector4 operator+(Vector4, Vector4);
    Vector4 operator-(Vector4, Vector4);
    Vector4 operator+(Vector4, float);
    Vector4 operator-(Vector4, float);
    Vector4 operator*(Vector4, float);
    Vector4 operator*(float, Vector4);
    Vector4 operator/(Vector4, float);
    bool operator==(Vector4, Vector4);
    bool operator!=(Vector4, Vector4);

    void swap(Vector4&, Vector4&);
} // namespace anton_engine

namespace anton_engine::math {
    inline float dot(Vector4 a, Vector4 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    inline float length_squared(Vector4 v) {
        return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    inline float length(Vector4 v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    }

    // If vector is non-zero, returns normalized copy of the vector.
    // Otherwise returns zero vector
    inline Vector4 normalize(Vector4 vec) {
        vec.normalize();
        return vec;
    }

    inline Vector4 multiply_componentwise(Vector4 const& a, Vector4 const& b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    }
} // namespace anton_engine::math

namespace anton_engine {
    inline float& Vector4::operator[](int index) {
        return (&x)[index];
    }

    inline float Vector4::operator[](int index) const {
        return (&x)[index];
    }

    inline Vector4& Vector4::operator-() {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
        return *this;
    }

    inline Vector4 Vector4::operator-() const {
        return {-x, -y, -z, -w};
    }

    inline Vector4& Vector4::operator+=(Vector4 a) {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
    }

    inline Vector4& Vector4::operator-=(Vector4 a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
    }

    inline Vector4& Vector4::operator+=(float a) {
        x += a;
        y += a;
        z += a;
        w += a;
        return *this;
    }

    inline Vector4& Vector4::operator-=(float a) {
        x -= a;
        y -= a;
        z -= a;
        w -= a;
        return *this;
    }

    inline Vector4& Vector4::operator*=(float a) {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
        return *this;
    }

    inline Vector4& Vector4::operator/=(float a) {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
        return *this;
    }

    inline bool Vector4::is_zero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
    }

    inline bool Vector4::is_almost_zero(float tolerance) const {
        return math::abs(x) <= tolerance && math::abs(y) <= tolerance && math::abs(z) <= tolerance && math::abs(w) <= tolerance;
    }

    inline float Vector4::length_squared() const {
        return x * x + y * y + z * z + w * w;
    }

    inline float Vector4::length() const {
        return std::sqrt(length_squared());
    }

    inline Vector4& Vector4::normalize() {
        if (!is_zero()) {
            float inverse_vec_length = math::inv_sqrt(length_squared());
            *this *= inverse_vec_length;
        }

        return *this;
    }

    inline Vector4& Vector4::multiply_componentwise(Vector4 const& a) {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        w *= a.w;
        return *this;
    }

    inline Vector4 operator+(Vector4 a, Vector4 b) {
        a += b;
        return a;
    }

    inline Vector4 operator-(Vector4 a, Vector4 b) {
        a -= b;
        return a;
    }

    inline Vector4 operator+(Vector4 a, float b) {
        a += b;
        return a;
    }

    inline Vector4 operator-(Vector4 a, float b) {
        a -= b;
        return a;
    }

    inline Vector4 operator*(Vector4 a, float b) {
        a *= b;
        return a;
    }

    inline Vector4 operator*(float b, Vector4 a) {
        a *= b;
        return a;
    }

    inline Vector4 operator/(Vector4 a, float b) {
        a /= b;
        return a;
    }

    inline bool operator==(Vector4 a, Vector4 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    inline bool operator!=(Vector4 a, Vector4 b) {
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
    }

    inline void swap(Vector4& a, Vector4& b) {
        anton_stl::swap(a.x, b.x);
        anton_stl::swap(a.y, b.y);
        anton_stl::swap(a.z, b.z);
        anton_stl::swap(a.w, b.w);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR4_HPP_INCLUDE
