#ifndef CORE_MATH_VECTOR2_HPP_INCLUDE
#define CORE_MATH_VECTOR2_HPP_INCLUDE

#include <anton_stl/utility.hpp>
#include <cmath>
#include <math/math.hpp>

namespace anton_engine {
    class Vector3;
    class Vector4;

    class Vector2 {
    public:
        static Vector2 const zero;
        static Vector2 const one;
        static Vector2 const up;
        static Vector2 const right;

        float x = 0;
        float y = 0;

        Vector2() = default;
        Vector2(float x, float y): x(x), y(y) {}
        explicit Vector2(Vector3 const&);
        explicit Vector2(Vector4 const&);

        float& operator[](int);
        float operator[](int) const;

        Vector2& operator-();
        Vector2 operator-() const;
        Vector2& operator+=(Vector2);
        Vector2& operator-=(Vector2);
        Vector2& operator+=(float);
        Vector2& operator-=(float);
        Vector2& operator*=(float);
        Vector2& operator/=(float);

        // Check if all components are equal 0
        bool is_zero() const;
        bool is_almost_zero(float tolerance = 0.00001f) const;

        float length_squared() const;
        float length() const;

        // If vector is non-zero, normalizes the vector.
        // Otherwise leaves it unchanged
        Vector2& normalize();

        Vector2& multiply_componentwise(Vector2 const&);
    };

    Vector2 operator+(Vector2, Vector2);
    Vector2 operator-(Vector2, Vector2);
    Vector2 operator+(Vector2, float);
    Vector2 operator-(Vector2, float);
    Vector2 operator*(Vector2, float);
    Vector2 operator*(float, Vector2);
    Vector2 operator/(Vector2, float);
    bool operator==(Vector2, Vector2);
    bool operator!=(Vector2, Vector2);

    void swap(Vector2&, Vector2&);
} // namespace anton_engine

namespace anton_engine::math {
    inline float dot(Vector2 const& vec1, Vector2 const& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y;
    }

    inline float length_squared(Vector2 v) {
        return v.x * v.x + v.y * v.y;
    }

    inline float length(Vector2 v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    // If vector is non-zero, returns normalized copy of the vector.
    // Otherwise returns zero vector
    inline Vector2 normalize(Vector2 vec) {
        vec.normalize();
        return vec;
    }

    inline Vector2 multiply_componentwise(Vector2 const& a, Vector2 const& b) {
        return {a.x * b.x, a.y * b.y};
    }
} // namespace anton_engine::math

namespace anton_engine {
    inline Vector2 const Vector2::zero = Vector2(0.0f, 0.0f);
    inline Vector2 const Vector2::one = Vector2(1.0f, 1.0f);
    inline Vector2 const Vector2::up = Vector2(0.0f, 1.0f);
    inline Vector2 const Vector2::right = Vector2(1.0f, 0.0f);

    inline float& Vector2::operator[](int index) {
        return (&x)[index];
    }

    inline float Vector2::operator[](int index) const {
        return (&x)[index];
    }

    inline Vector2& Vector2::operator-() {
        x = -x;
        y = -y;
        return *this;
    }

    inline Vector2 Vector2::operator-() const {
        return {-x, -y};
    }

    inline Vector2& Vector2::operator+=(Vector2 a) {
        x += a.x;
        y += a.y;
        return *this;
    }

    inline Vector2& Vector2::operator-=(Vector2 a) {
        x -= a.x;
        y -= a.y;
        return *this;
    }

    inline Vector2& Vector2::operator+=(float a) {
        x += a;
        y += a;
        return *this;
    }

    inline Vector2& Vector2::operator-=(float a) {
        x -= a;
        y -= a;
        return *this;
    }

    inline Vector2& Vector2::operator*=(float a) {
        x *= a;
        y *= a;
        return *this;
    }

    inline Vector2& Vector2::operator/=(float a) {
        x /= a;
        y /= a;
        return *this;
    }

    inline bool Vector2::is_zero() const {
        return x == 0.0f && y == 0.0f;
    }

    inline bool Vector2::is_almost_zero(float tolerance) const {
        return math::abs(x) <= tolerance && math::abs(y) <= tolerance;
    }

    inline float Vector2::length_squared() const {
        return x * x + y * y;
    }

    inline float Vector2::length() const {
        return std::sqrt(x * x + y * y);
    }

    inline Vector2& Vector2::normalize() {
        if (!is_zero()) {
            float inverse_vec_length = math::inv_sqrt(length_squared());
            *this *= inverse_vec_length;
        }
        return *this;
    }

    inline Vector2& Vector2::multiply_componentwise(Vector2 const& a) {
        x *= a.x;
        y *= a.y;
        return *this;
    }

    inline Vector2 operator+(Vector2 a, Vector2 b) {
        a += b;
        return a;
    }

    inline Vector2 operator-(Vector2 a, Vector2 b) {
        a -= b;
        return a;
    }

    inline Vector2 operator+(Vector2 a, float b) {
        a += b;
        return a;
    }

    inline Vector2 operator-(Vector2 a, float b) {
        a -= b;
        return a;
    }

    inline Vector2 operator*(Vector2 a, float b) {
        a *= b;
        return a;
    }

    inline Vector2 operator*(float b, Vector2 a) {
        a *= b;
        return a;
    }

    inline Vector2 operator/(Vector2 a, float b) {
        a /= b;
        return a;
    }

    inline bool operator==(Vector2 a, Vector2 b) {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator!=(Vector2 a, Vector2 b) {
        return a.x != b.x || a.y != b.y;
    }

    inline void swap(Vector2& a, Vector2& b) {
        anton_stl::swap(a.x, b.x);
        anton_stl::swap(a.y, b.y);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR2_HPP_INCLUDE
