#ifndef CORE_MATH_VECTOR3_HPP_INCLUDE
#define CORE_MATH_VECTOR3_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

namespace anton_engine {
    class Vector2;
    class Vector4;

    class Vector3 {
    public:
        f32 x = 0;
        f32 y = 0;
        f32 z = 0;

        Vector3() = default;
        Vector3(f32 x, f32 y, f32 z): x(x), y(y), z(z) {}
        explicit Vector3(Vector2 const&, f32 z = 0.0f);
        explicit Vector3(Vector4 const&);

        f32& operator[](i32 index) {
            return (&x)[index];
        }

        f32 operator[](i32 index) const {
            return (&x)[index];
        }
    };

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

    // Componentwise multiply.
    //
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

    // Componentwise multiply.
    //
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

    namespace math {
        inline bool is_almost_zero(Vector3 const v, f32 const tolerance = 0.000001f) {
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

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector.
        inline Vector3 normalize(Vector3 vec) {
            if(!is_almost_zero(vec)) {
                f32 inverse_vec_length = math::inv_sqrt(length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }

        inline Vector3 lerp(Vector3 const a, Vector3 const b, f32 const t) {
            return (1.0f - t) * a + t * b;
        }
    } // namespace math

    inline void swap(Vector3& a, Vector3& b) {
        atl::swap(a.x, b.x);
        atl::swap(a.y, b.y);
        atl::swap(a.z, b.z);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE
