#ifndef CORE_MATH_VECTOR3_HPP_INCLUDE
#define CORE_MATH_VECTOR3_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

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

        f32 x = 0;
        f32 y = 0;
        f32 z = 0;

        Vector3() = default;
        Vector3(f32 x, f32 y, f32 z): x(x), y(y), z(z) {}
        explicit Vector3(Vector2 const&, f32 z = 0);
        explicit Vector3(Vector4 const&);

        f32& operator[](i32);
        f32 operator[](i32) const;
    };

    Vector3 operator-(Vector3 const& v);
    Vector3& operator+=(Vector3 const& v, Vector3);
    Vector3& operator-=(Vector3 const& v, Vector3);
    // Componentwise multiply
    Vector3& operator*=(Vector3 const& v, Vector3);
    // Componentwise divide
    Vector3& operator/=(Vector3 const& v, Vector3);
    Vector3& operator+=(Vector3 const& v, f32);
    Vector3& operator-=(Vector3 const& v, f32);
    Vector3& operator*=(Vector3 const& v, f32);
    Vector3& operator/=(Vector3 const& v, f32);

    Vector3 operator+(Vector3, Vector3);
    Vector3 operator-(Vector3, Vector3);
    // Componentwise multiply
    Vector3 operator*(Vector3, Vector3);
    // Componentwise divide
    Vector3 operator/(Vector3, Vector3);
    Vector3 operator+(Vector3, f32);
    Vector3 operator-(Vector3, f32);
    Vector3 operator*(Vector3, f32);
    Vector3 operator*(f32, Vector3);
    Vector3 operator/(Vector3, f32);
    bool operator==(Vector3, Vector3);
    bool operator!=(Vector3, Vector3);

    void swap(Vector3&, Vector3&);

    namespace math {
        bool is_almost_zero(Vector3, f32 tolerance = 0.000001f);

        f32 dot(Vector3, Vector3);
        Vector3 cross(Vector3, Vector3);
        f32 length_squared(Vector3);
        f32 length(Vector3);

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector
        Vector3 normalize(Vector3);
    } // namespace math
} // namespace anton_engine

namespace anton_engine {
    inline Vector3 const Vector3::zero = Vector3(0, 0, 0);
    inline Vector3 const Vector3::one = Vector3(1, 1, 1);
    inline Vector3 const Vector3::up = Vector3(0, 1, 0);
    inline Vector3 const Vector3::forward = Vector3(0, 0, -1);
    inline Vector3 const Vector3::right = Vector3(1, 0, 0);

    inline f32& Vector3::operator[](int index) {
        return (&x)[index];
    }

    inline f32 Vector3::operator[](int index) const {
        return (&x)[index];
    }

    inline Vector3 operator-(Vector3 const& v) {
        return {-v.x, -v.y, -v.z};
    }

    inline Vector3& operator+=(Vector3& v, Vector3 a) {
        v.x += a.x;
        v.y += a.y;
        v.z += a.z;
        return v;
    }

    inline Vector3& operator-=(Vector3& v, Vector3 a) {
        v.x -= a.x;
        v.y -= a.y;
        v.z -= a.z;
        return v;
    }

    inline Vector3& operator*=(Vector3& v, Vector3 a) {
        v.x *= a.x;
        v.y *= a.y;
        v.z *= a.z;
        return v;
    }

    inline Vector3& operator/=(Vector3& v, Vector3 a) {
        v.x /= a.x;
        v.y /= a.y;
        v.z /= a.z;
        return v;
    }

    inline Vector3& operator+=(Vector3& v, f32 a) {
        v.x += a;
        v.y += a;
        v.z += a;
        return v;
    }

    inline Vector3& operator-=(Vector3& v, f32 a) {
        v.x -= a;
        v.y -= a;
        v.z -= a;
        return v;
    }

    inline Vector3& operator*=(Vector3& v, f32 a) {
        v.x *= a;
        v.y *= a;
        v.z *= a;
        return v;
    }

    inline Vector3& operator/=(Vector3& v, f32 a) {
        v.x /= a;
        v.y /= a;
        v.z /= a;
        return v;
    }

    inline Vector3 operator+(Vector3 a, Vector3 b) {
        a += b;
        return a;
    }

    inline Vector3 operator-(Vector3 a, Vector3 b) {
        a -= b;
        return a;
    }

    inline Vector3 operator*(Vector3 a, Vector3 b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z};
    }

    inline Vector3 operator/(Vector3 a, Vector3 b) {
        return {a.x / b.x, a.y / b.y, a.z / b.z};
    }

    inline Vector3 operator+(Vector3 a, f32 b) {
        a += b;
        return a;
    }

    inline Vector3 operator-(Vector3 a, f32 b) {
        a -= b;
        return a;
    }

    inline Vector3 operator*(Vector3 a, f32 b) {
        a *= b;
        return a;
    }

    inline Vector3 operator*(f32 b, Vector3 a) {
        a *= b;
        return a;
    }

    inline Vector3 operator/(Vector3 a, f32 b) {
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
        atl::swap(a.x, b.x);
        atl::swap(a.y, b.y);
        atl::swap(a.z, b.z);
    }

    namespace math {
        inline bool is_almost_zero(Vector3 const v, f32 const tolerance) {
            return math::abs(v.x) <= tolerance && math::abs(v.y) <= tolerance && math::abs(v.z) <= tolerance;
        }

        inline f32 dot(Vector3 const v1, Vector3 const v2) {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
        }

        inline Vector3 cross(Vector3 const v1, Vector3 const v2) {
            return Vector3(v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
        }

        inline f32 length_squared(Vector3 const v) {
            return v.x * v.x + v.y * v.y + v.z * v.z;
        }

        inline f32 length(Vector3 v) {
            return math::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        }

        inline Vector3 normalize(Vector3 vec) {
            if (!is_almost_zero(vec)) {
                f32 inverse_vec_length = math::inv_sqrt(length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE
