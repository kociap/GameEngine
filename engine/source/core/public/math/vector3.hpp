#ifndef CORE_MATH_VECTOR3_HPP_INCLUDE
#define CORE_MATH_VECTOR3_HPP_INCLUDE

#include <anton_stl/utility.hpp>
#include <cmath>
#include <math/math.hpp>

namespace anton_engine {
    class Vector2;
    class Vector4;

    class Vector3 {
    public:
        static Vector3 const zero;
        static Vector3 const one;
        static Vector3 const up;
        static Vector3 const right;
        static Vector3 const forward;

        float x = 0;
        float y = 0;
        float z = 0;

        Vector3() = default;
        Vector3(float x, float y, float z): x(x), y(y), z(z) {}
        explicit Vector3(Vector2 const&, float z = 0);
        explicit Vector3(Vector4 const&);

        float& operator[](int);
        float operator[](int) const;

        Vector3& operator-();
        Vector3 operator-() const;
        Vector3& operator+=(Vector3);
        Vector3& operator-=(Vector3);
        Vector3& operator+=(float);
        Vector3& operator-=(float);
        Vector3& operator*=(float);
        Vector3& operator/=(float);

        // Check if all components are equal 0
        bool is_zero() const;
        bool is_almost_zero(float tolerance = 0.00001f) const;

        float length_squared() const;
        float length() const;

        // If vector is non-zero, normalizes the vector.
        // Otherwise leaves it unchanged
        Vector3& normalize();

        Vector3& multiply_componentwise(Vector3 const&);
    };

    Vector3 operator+(Vector3, Vector3);
    Vector3 operator-(Vector3, Vector3);
    Vector3 operator+(Vector3, float);
    Vector3 operator-(Vector3, float);
    Vector3 operator*(Vector3, float);
    Vector3 operator*(float, Vector3);
    Vector3 operator/(Vector3, float);
    bool operator==(Vector3, Vector3);
    bool operator!=(Vector3, Vector3);

    void swap(Vector3&, Vector3&);
} // namespace anton_engine

namespace anton_engine::math {
    inline float dot(Vector3 const& vec1, Vector3 const& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    inline Vector3 cross(Vector3 const& vec1, Vector3 const& vec2) {
        return Vector3(vec1.y * vec2.z - vec2.y * vec1.z, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x);
    }

    inline float length_squared(Vector3 v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    inline float length(Vector3 v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    // If vector is non-zero, returns normalized copy of the vector.
    // Otherwise returns zero vector
    inline Vector3 normalize(Vector3 vec) {
        vec.normalize();
        return vec;
    }

    inline Vector3 multiply_componentwise(Vector3 const& a, Vector3 const& b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z};
    }
} // namespace anton_engine::math

namespace anton_engine {
    inline Vector3 const Vector3::zero = Vector3(0, 0, 0);
    inline Vector3 const Vector3::one = Vector3(1, 1, 1);
    inline Vector3 const Vector3::up = Vector3(0, 1, 0);
    inline Vector3 const Vector3::forward = Vector3(0, 0, -1);
    inline Vector3 const Vector3::right = Vector3(1, 0, 0);

    inline float& Vector3::operator[](int index) {
        return (&x)[index];
    }

    inline float Vector3::operator[](int index) const {
        return (&x)[index];
    }

    inline Vector3& Vector3::operator-() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    inline Vector3 Vector3::operator-() const {
        return {-x, -y, -z};
    }

    inline Vector3& Vector3::operator+=(Vector3 a) {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }

    inline Vector3& Vector3::operator-=(Vector3 a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }

    inline Vector3& Vector3::operator+=(float a) {
        x += a;
        y += a;
        z += a;
        return *this;
    }

    inline Vector3& Vector3::operator-=(float a) {
        x -= a;
        y -= a;
        z -= a;
        return *this;
    }

    inline Vector3& Vector3::operator*=(float a) {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }

    inline Vector3& Vector3::operator/=(float a) {
        x /= a;
        y /= a;
        z /= a;
        return *this;
    }

    inline bool Vector3::is_zero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }

    inline bool Vector3::is_almost_zero(float tolerance) const {
        return math::abs(x) <= tolerance && math::abs(y) <= tolerance && math::abs(z) <= tolerance;
    }

    inline float Vector3::length_squared() const {
        return x * x + y * y + z * z;
    }

    inline float Vector3::length() const {
        return std::sqrt(length_squared());
    }

    inline Vector3& Vector3::normalize() {
        if (!is_zero()) {
            float inverse_vec_length = math::inv_sqrt(length_squared());
            *this *= inverse_vec_length;
        }
        return *this;
    }

    inline Vector3& Vector3::multiply_componentwise(Vector3 const& a) {
        x *= a.x;
        y *= a.y;
        z *= a.z;
        return *this;
    }

    inline Vector3 operator+(Vector3 a, Vector3 b) {
        a += b;
        return a;
    }

    inline Vector3 operator-(Vector3 a, Vector3 b) {
        a -= b;
        return a;
    }

    inline Vector3 operator+(Vector3 a, float b) {
        a += b;
        return a;
    }

    inline Vector3 operator-(Vector3 a, float b) {
        a -= b;
        return a;
    }

    inline Vector3 operator*(Vector3 a, float b) {
        a *= b;
        return a;
    }

    inline Vector3 operator*(float b, Vector3 a) {
        a *= b;
        return a;
    }

    inline Vector3 operator/(Vector3 a, float b) {
        a /= b;
        return a;
    }

    inline bool operator==(Vector3 a, Vector3 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    inline bool operator!=(Vector3 a, Vector3 b) {
        return a.x != b.x || a.y != b.y || a.z != b.z;
    }

    inline void swap(Vector3& a, Vector3& b) {
        anton_stl::swap(a.x, b.x);
        anton_stl::swap(a.y, b.y);
        anton_stl::swap(a.z, b.z);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE
