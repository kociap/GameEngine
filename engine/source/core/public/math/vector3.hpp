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

        Vector3 operator-() const;
        Vector3& operator+=(Vector3);
        Vector3& operator-=(Vector3);
        Vector3& operator+=(float);
        Vector3& operator-=(float);
        Vector3& operator*=(float);
        Vector3& operator/=(float);
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

    namespace math {
        bool is_almost_zero(Vector3, float tolerance = 0.000001f);

        float dot(Vector3, Vector3);
        Vector3 cross(Vector3, Vector3);
        float length_squared(Vector3);
        float length(Vector3);

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector
        Vector3 normalize(Vector3);

        Vector3 multiply_componentwise(Vector3, Vector3);
    } // namespace math
} // namespace anton_engine

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

    namespace math {
        inline bool is_almost_zero(Vector3 const v, float const tolerance) {
            return math::abs(v.x) <= tolerance && math::abs(v.y) <= tolerance && math::abs(v.z) <= tolerance;
        }

        inline float dot(Vector3 const v1, Vector3 const v2) {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }

        inline Vector3 cross(Vector3 const v1, Vector3 const v2) {
            return Vector3(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
        }

        inline float length_squared(Vector3 const v) {
            return v.x * v.x + v.y * v.y + v.z * v.z;
        }

        inline float length(Vector3 v) {
            return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        inline Vector3 normalize(Vector3 vec) {
            if (!is_almost_zero(vec)) {
                float inverse_vec_length = math::inv_sqrt(length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }

        inline Vector3 multiply_componentwise(Vector3 const a, Vector3 const b) {
            return {a.x * b.x, a.y * b.y, a.z * b.z};
        }
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE
