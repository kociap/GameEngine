#ifndef CORE_MATH_VECTOR2_HPP_INCLUDE
#define CORE_MATH_VECTOR2_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

namespace anton_engine {
    class Vector3;
    class Vector4;

    class Vector2 {
    public:
        static Vector2 const zero;
        static Vector2 const one;
        static Vector2 const up;
        static Vector2 const right;

        f32 x = 0;
        f32 y = 0;

        Vector2() = default;
        Vector2(f32 x, f32 y): x(x), y(y) {}
        explicit Vector2(Vector3 const&);
        explicit Vector2(Vector4 const&);

        f32& operator[](i32);
        f32 operator[](i32) const;
    };

    Vector2 operator-(Vector2 const& v);
    Vector2& operator+=(Vector2& v, Vector2);
    Vector2& operator-=(Vector2& v, Vector2);
    // Componentwise multiply
    Vector2& operator*=(Vector2& v, Vector2);
    // Componentwise divide
    Vector2& operator/=(Vector2& v, Vector2);
    Vector2& operator-=(Vector2& v, f32);
    Vector2& operator*=(Vector2& v, f32);
    Vector2& operator/=(Vector2& v, f32);

    Vector2 operator+(Vector2, Vector2);
    Vector2 operator-(Vector2, Vector2);
    // Componentwise multiply
    Vector2 operator*(Vector2, Vector2);
    // Componentwise divide
    Vector2 operator/(Vector2, Vector2);
    Vector2& operator+=(Vector2& v, f32);
    Vector2 operator+(Vector2, f32);
    Vector2 operator-(Vector2, f32);
    Vector2 operator*(Vector2, f32);
    Vector2 operator*(f32, Vector2);
    Vector2 operator/(Vector2, f32);
    bool operator==(Vector2, Vector2);
    bool operator!=(Vector2, Vector2);

    void swap(Vector2&, Vector2&);

    namespace math {
        bool is_almost_zero(Vector2, f32 tolerance = 0.000001f);

        f32 dot(Vector2, Vector2);
        f32 length_squared(Vector2);
        f32 length(Vector2);

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector
        Vector2 normalize(Vector2);
    } // namespace math

    class IVector2 {
    public:
        i64 x = 0;
        i64 y = 0;

        IVector2() = default;
        IVector2(i64 x, i64 y): x(x), y(y) {}
        explicit IVector2(Vector2 const v): x(v.x), y(v.y) {}

        explicit operator Vector2() {
            return Vector2(x, y);
        }

        i64& operator[](i32);
        i64 operator[](i32) const;

        IVector2 operator-() const;
        IVector2& operator+=(IVector2);
        IVector2& operator-=(IVector2);
        IVector2& operator+=(i64);
        IVector2& operator-=(i64);
        IVector2& operator*=(i64);
        IVector2& operator/=(i64);
    };

    IVector2 operator+(IVector2, IVector2);
    IVector2 operator-(IVector2, IVector2);
    IVector2 operator+(IVector2, i64);
    IVector2 operator-(IVector2, i64);
    IVector2 operator*(IVector2, i64);
    IVector2 operator*(i64, IVector2);
    IVector2 operator/(IVector2, i64);
    bool operator==(IVector2, IVector2);
    bool operator!=(IVector2, IVector2);

    void swap(IVector2&, IVector2&);
} // namespace anton_engine

namespace anton_engine {
    inline Vector2 const Vector2::zero = Vector2(0.0f, 0.0f);
    inline Vector2 const Vector2::one = Vector2(1.0f, 1.0f);
    inline Vector2 const Vector2::up = Vector2(0.0f, 1.0f);
    inline Vector2 const Vector2::right = Vector2(1.0f, 0.0f);

    inline f32& Vector2::operator[](i32 index) {
        return (&x)[index];
    }

    inline f32 Vector2::operator[](i32 index) const {
        return (&x)[index];
    }

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

    inline Vector2& operator*=(Vector2& v, Vector2 a) {
        v.x *= a.x;
        v.y *= a.y;
        return v;
    }

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

    inline Vector2 operator*(Vector2 a, Vector2 b) {
        return {a.x * b.x, a.y * b.y};
    }

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

    inline void swap(Vector2& a, Vector2& b) {
        atl::swap(a.x, b.x);
        atl::swap(a.y, b.y);
    }

    namespace math {
        inline bool is_almost_zero(Vector2 const v, f32 const tolerance) {
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
        // Otherwise returns zero vector
        inline Vector2 normalize(Vector2 vec) {
            if (!is_almost_zero(vec)) {
                f32 inverse_vec_length = math::inv_sqrt(math::length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }
    } // namespace math

    // IVector2

    inline i64& IVector2::operator[](i32 const index) {
        return (&x)[index];
    }

    inline i64 IVector2::operator[](i32 const index) const {
        return (&x)[index];
    }

    inline IVector2 IVector2::operator-() const {
        return {-x, -y};
    }

    inline IVector2& IVector2::operator+=(IVector2 const v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    inline IVector2& IVector2::operator-=(IVector2 const v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline IVector2& IVector2::operator+=(i64 const a) {
        x += a;
        y += a;
        return *this;
    }

    inline IVector2& IVector2::operator-=(i64 const a) {
        x -= a;
        y -= a;
        return *this;
    }

    inline IVector2& IVector2::operator*=(i64 const a) {
        x *= a;
        y *= a;
        return *this;
    }

    inline IVector2& IVector2::operator/=(i64 const a) {
        x /= a;
        y /= a;
        return *this;
    }

    inline IVector2 operator+(IVector2 const v1, IVector2 const v2) {
        return {v1.x + v2.x, v1.y + v2.y};
    }

    inline IVector2 operator-(IVector2 const v1, IVector2 const v2) {
        return {v1.x - v2.x, v1.y - v2.y};
    }

    inline IVector2 operator+(IVector2 const v, i64 const a) {
        return {v.x + a, v.y + a};
    }

    inline IVector2 operator-(IVector2 const v, i64 const a) {
        return {v.x - a, v.y - a};
    }

    inline IVector2 operator*(IVector2 const v, i64 const a) {
        return {v.x * a, v.y * a};
    }

    inline IVector2 operator*(i64 const a, IVector2 const v) {
        return {v.x * a, v.y * a};
    }

    inline IVector2 operator/(IVector2 const v, i64 const a) {
        return {v.x / a, v.y / a};
    }

    inline bool operator==(IVector2 const v1, IVector2 const v2) {
        return v1.x == v2.x && v1.y == v2.y;
    }

    inline bool operator!=(IVector2 const v1, IVector2 const v2) {
        return v1.x != v2.x || v1.y != v2.y;
    }

    inline void swap(IVector2& v1, IVector2& v2) {
        atl::swap(v1.x, v2.x);
        atl::swap(v1.y, v2.y);
    }
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR2_HPP_INCLUDE
