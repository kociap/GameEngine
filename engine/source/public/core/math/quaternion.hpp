#ifndef CORE_MATH_QUATERNION_HPP_INCLUDE
#define CORE_MATH_QUATERNION_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/math.hpp>

namespace anton_engine {
    class Quaternion {
    public:
        // Identity quaternion.
        // Equivalent to Quaternion(0, 0, 0, 1)
        static Quaternion const identity;

        f32 x = 0;
        f32 y = 0;
        f32 z = 0;
        f32 w = 1;

        Quaternion() = default;
        Quaternion(f32 x, f32 y, f32 z, f32 w): x(x), y(y), z(z), w(w) {}
    };

    inline Quaternion const Quaternion::identity = Quaternion(0, 0, 0, 1);

    inline Quaternion operator+(Quaternion const& q1, Quaternion const& q2) {
        return {q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w};
    }

    inline Quaternion operator-(Quaternion const& q1, Quaternion const& q2) {
        return {q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w};
    }

    // Hamilton Product
    //
    inline Quaternion operator*(Quaternion p, Quaternion q) {
        // clang-format off
        return {p.w * q.x + q.w * p.x + p.y * q.z - p.z * q.y,
                p.w * q.y + q.w * p.y + p.z * q.x - p.x * q.z,
                p.w * q.z + q.w * p.z + p.x * q.y - p.y * q.x,
                p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z};
        // clang-format on
    }

    inline Quaternion operator*(Quaternion const& q, f32 a) {
        return {q.x * a, q.y * a, q.z * a, q.w * a};
    }

    inline Quaternion operator/(Quaternion const& q, f32 a) {
        return {q.x / a, q.y / a, q.z / a, q.w / a};
    }

    inline f32 length_squared(Quaternion const& q) {
        return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    }

    namespace math {
        inline f32 length(Quaternion const& q) {
            return math::sqrt(length_squared(q));
        }

        inline Quaternion normalize(Quaternion const& q) {
            return q * math::inv_sqrt(length_squared(q));
        }

        inline Quaternion conjugate(Quaternion const& q) {
            return {-q.x, -q.y, -q.z, q.w};
        }

        // If quaternion is normalized, this function returns the same result as conjugate.
        //
        inline Quaternion inverse(Quaternion const& q) {
            return conjugate(q) / length_squared(q);
        }
    } // namespace math

    inline void swap(Quaternion& q1, Quaternion& q2) {
        atl::swap(q1.x, q2.x);
        atl::swap(q1.y, q2.y);
        atl::swap(q1.z, q2.z);
        atl::swap(q1.w, q2.w);
    }
} // namespace anton_engine

#endif // !CORE_MATH_QUATERNION_HPP_INCLUDE
