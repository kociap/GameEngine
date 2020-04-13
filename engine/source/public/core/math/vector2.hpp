#ifndef CORE_MATH_VECTOR2_HPP_INCLUDE
#define CORE_MATH_VECTOR2_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

namespace anton_engine {
    class Vector3;
    class Vector4;

    class Vector2 {
    public:
        f32 x = 0;
        f32 y = 0;

        Vector2() = default;
        Vector2(f32 x, f32 y): x(x), y(y) {}
        explicit Vector2(Vector3 const&);
        explicit Vector2(Vector4 const&);

        f32& operator[](i32 index) {
            return (&x)[index];
        }

        f32 operator[](i32 index) const {
            return (&x)[index];
        }
    };

    inline Vector2 operator-(Vector2 const& v) {
        return {-v.x, -v.y};
    }

    inline Vector2& operator+=(Vector2& v, Vector2 a) {
        v.x += a.x;
        v.y += a.y;
        return v;
    }

    inline Vector2& operator-=(Vector2& v, Vector2 a) {
        v.x -= a.x;
        v.y -= a.y;
        return v;
    }

    inline Vector2& operator+=(Vector2& v, f32 a) {
        v.x += a;
        v.y += a;
        return v;
    }

    // Componentwise multiply.
    //
    inline Vector2& operator*=(Vector2& v, Vector2 a) {
        v.x *= a.x;
        v.y *= a.y;
        return v;
    }

    // Componentwise divide.
    //
    inline Vector2& operator/=(Vector2& v, Vector2 a) {
        v.x /= a.x;
        v.y /= a.y;
        return v;
    }

    inline Vector2& operator-=(Vector2& v, f32 a) {
        v.x -= a;
        v.y -= a;
        return v;
    }

    inline Vector2& operator*=(Vector2& v, f32 a) {
        v.x *= a;
        v.y *= a;
        return v;
    }

    inline Vector2& operator/=(Vector2& v, f32 a) {
        v.x /= a;
        v.y /= a;
        return v;
    }

    inline Vector2 operator+(Vector2 a, Vector2 b) {
        a += b;
        return a;
    }

    inline Vector2 operator-(Vector2 a, Vector2 b) {
        a -= b;
        return a;
    }

    // Componentwise multiply.
    //
    inline Vector2 operator*(Vector2 a, Vector2 b) {
        return {a.x * b.x, a.y * b.y};
    }

    // Componentwise divide.
    //
    inline Vector2 operator/(Vector2 a, Vector2 b) {
        return {a.x / b.x, a.y / b.y};
    }

    inline Vector2 operator+(Vector2 a, f32 b) {
        a += b;
        return a;
    }

    inline Vector2 operator-(Vector2 a, f32 b) {
        a -= b;
        return a;
    }

    inline Vector2 operator*(Vector2 a, f32 b) {
        a *= b;
        return a;
    }

    inline Vector2 operator*(f32 b, Vector2 a) {
        a *= b;
        return a;
    }

    inline Vector2 operator/(Vector2 a, f32 b) {
        a /= b;
        return a;
    }

    inline bool operator==(Vector2 a, Vector2 b) {
        return a.x == b.x && a.y == b.y;
    }

    inline bool operator!=(Vector2 a, Vector2 b) {
        return a.x != b.x || a.y != b.y;
    }

    namespace math {
        inline bool is_almost_zero(Vector2 const v, f32 const tolerance = 0.000001f) {
            return math::abs(v.x) <= tolerance && math::abs(v.y) <= tolerance;
        }

        inline f32 dot(Vector2 const vec1, Vector2 const vec2) {
            return vec1.x * vec2.x + vec1.y * vec2.y;
        }

        inline f32 length_squared(Vector2 const v) {
            return v.x * v.x + v.y * v.y;
        }

        inline f32 length(Vector2 const v) {
            return math::sqrt(v.x * v.x + v.y * v.y);
        }

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector.
        inline Vector2 normalize(Vector2 vec) {
            if (!is_almost_zero(vec)) {
                f32 inverse_vec_length = math::inv_sqrt(math::length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }
    } // namespace math

    inline void swap(Vector2& a, Vector2& b) {
        atl::swap(a.x, b.x);
        atl::swap(a.y, b.y);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR2_HPP_INCLUDE
