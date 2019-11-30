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

        Vector4 operator-() const;
        Vector4& operator+=(Vector4);
        Vector4& operator-=(Vector4);
        Vector4& operator+=(float);
        Vector4& operator-=(float);
        Vector4& operator*=(float);
        Vector4& operator/=(float);
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

    namespace math {
        bool is_almost_zero(Vector4, float tolerance = 0.000001f);

        float dot(Vector4, Vector4);
        float length_squared(Vector4);
        float length(Vector4);

        // If vector is non-zero, returns normalized copy of the vector.
        // Otherwise returns zero vector
        Vector4 normalize(Vector4);

        Vector4 multiply_componentwise(Vector4, Vector4);
    } // namespace math
} // namespace anton_engine

namespace anton_engine {
    inline float& Vector4::operator[](int index) {
        return (&x)[index];
    }

    inline float Vector4::operator[](int index) const {
        return (&x)[index];
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

    namespace math {
        inline bool is_almost_zero(Vector4 const v, float const tolerance) {
            return math::abs(v.x) <= tolerance && math::abs(v.y) <= tolerance && math::abs(v.z) <= tolerance && math::abs(v.w) <= tolerance;
        }

        inline float dot(Vector4 const v1, Vector4 const v2) {
            return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
        }

        inline float length_squared(Vector4 const v) {
            return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
        }

        inline float length(Vector4 const v) {
            return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
        }

        inline Vector4 normalize(Vector4 vec) {
            if (!is_almost_zero(vec)) {
                float inverse_vec_length = math::inv_sqrt(length_squared(vec));
                vec *= inverse_vec_length;
            }

            return vec;
        }

        inline Vector4 multiply_componentwise(Vector4 const v1, Vector4 const v2) {
            return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w};
        }
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_VECTOR4_HPP_INCLUDE
