#ifndef CORE_MATH_VECTOR4_HPP_INCLUDE
#define CORE_MATH_VECTOR4_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

namespace anton_engine {
    class Vector2;
    class Vector3;

    class Vector4 {
    public:
        f32 x = 0;
        f32 y = 0;
        f32 z = 0;
        f32 w = 0;

        Vector4() = default;
        Vector4(f32 x, f32 y, f32 z, f32 w): x(x), y(y), z(z), w(w) {}
        explicit Vector4(Vector2 const&, f32 z = 0.0f, f32 w = 0.0f);
        explicit Vector4(Vector3 const&, f32 w = 0.0f);

        f32& operator[](i32 index) {
            return (&x)[index];
        }

        f32 operator[](i32 index) const {
            return (&x)[index];
        }
    };

    inline Vector4 operator-(Vector4 const& v) {
        return {-v.x, -v.y, -v.z, -v.w};
    }

    inline Vector4& operator+=(Vector4& v, Vector4 a) {
        v.x += a.x;
        v.y += a.y;
        v.z += a.z;
        v.w += a.w;
        return v;
    }

    inline Vector4& operator-=(Vector4& v, Vector4 a) {
        v.x -= a.x;
        v.y -= a.y;
        v.z -= a.z;
        v.w -= a.w;
        return v;
    }

    // Componentwise multiply.
    //
    inline Vector4& operator*=(Vector4& v, Vector4 a) {
        v.x *= a.x;
        v.y *= a.y;
        v.z *= a.z;
        v.w *= a.w;
        return v;
    }

    // Componentwise divide.
    //
    inline Vector4& operator/=(Vector4& v, Vector4 a) {
        v.x /= a.x;
        v.y /= a.y;
        v.z /= a.z;
        v.w /= a.w;
        return v;
    }

    inline Vector4& operator+=(Vector4& v, f32 a) {
        v.x += a;
        v.y += a;
        v.z += a;
        v.w += a;
        return v;
    }

    inline Vector4& operator-=(Vector4& v, f32 a) {
        v.x -= a;
        v.y -= a;
        v.z -= a;
        v.w -= a;
        return v;
    }

    inline Vector4& operator*=(Vector4& v, f32 a) {
        v.x *= a;
        v.y *= a;
        v.z *= a;
        v.w *= a;
        return v;
    }

    inline Vector4& operator/=(Vector4& v, f32 a) {
        v.x /= a;
        v.y /= a;
        v.z /= a;
        v.w /= a;
        return v;
    }

    inline Vector4 operator+(Vector4 a, Vector4 b) {
        a += b;
        return a;
    }

    inline Vector4 operator-(Vector4 a, Vector4 b) {
        a -= b;
        return a;
    }

    // Componentwise multiply.
    //
    inline Vector4 operator*(Vector4 a, Vector4 b) {
        return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    }

    // Componentwise divide.
    //
    inline Vector4 operator/(Vector4 a, Vector4 b) {
        return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
    }

    inline Vector4 operator+(Vector4 a, f32 b) {
        a += b;
        return a;
    }

    inline Vector4 operator-(Vector4 a, f32 b) {
        a -= b;
        return a;
    }

    inline Vector4 operator*(Vector4 a, f32 b) {
        a *= b;
        return a;
    }

    inline Vector4 operator*(f32 b, Vector4 a) {
        a *= b;
        return a;
    }

    inline Vector4 operator/(Vector4 a, f32 b) {
        a /= b;
        return a;
    }

    inline bool operator==(Vector4 a, Vector4 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    inline bool operator!=(Vector4 a, Vector4 b) {
        return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
    }

    namespace math {
        inline bool is_almost_zero(Vector4 const v, f32 const tolerance = 0.000001f) {
            return math::abs(v.x) <= tolerance && math::abs(v.y) <= tolerance && math::abs(v.z) <= tolerance && math::abs(v.w) <= tolerance;
        }

        inline f32 dot(Vector4 const v1, Vector4 const v2) {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }

        inline f32 length_squared(Vector4 const v) {
            return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
        }

        inline f32 length(Vector4 const v) {
            return math::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector.
        inline Vector4 normalize(Vector4 vec) {
            if (!is_almost_zero(vec)) {
                f32 inverse_vec_length = math::inv_sqrt(length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }
    } // namespace math

    inline void swap(Vector4& a, Vector4& b) {
        atl::swap(a.x, b.x);
        atl::swap(a.y, b.y);
        atl::swap(a.z, b.z);
        atl::swap(a.w, b.w);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR4_HPP_INCLUDE
