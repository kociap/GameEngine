#ifndef CORE_MATH_QUATERNION_HPP_INCLUDE
#define CORE_MATH_QUATERNION_HPP_INCLUDE

#include <core/math/math.hpp>

namespace anton_engine {
    class Quaternion {
    public:
        // Identity quaternion.
        // Equivalent to Quaternion(0, 0, 0, 1)
        static Quaternion const identity;

    public:
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 1;

    public:
        Quaternion() = default;
        Quaternion(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}

        // Make this quaternion conjugate
        void conjugate();
        // Normalize this quaternion
        void normalize();
        // Make this quaternion an inverse
        void inverse();
    };

    Quaternion operator+(Quaternion const&, Quaternion const&);
    Quaternion operator-(Quaternion const&, Quaternion const&);
    // Hamilton Product
    Quaternion operator*(Quaternion, Quaternion);
    Quaternion operator*(Quaternion const&, float);
    Quaternion operator/(Quaternion const&, float);
} // namespace anton_engine

namespace anton_engine::math {
    inline float length_squared(Quaternion const& q) {
        return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    }

    inline float length(Quaternion const& q) {
        return sqrt(length_squared(q));
    }

    inline Quaternion normalize(Quaternion const& q) {
        return q * math::inv_sqrt(length_squared(q));
    }

    inline Quaternion conjugate(Quaternion const& q) {
        return {-q.x, -q.y, -q.z, q.w};
    }

    // If quaternion is normalized, this function returns the same result as conjugate
    inline Quaternion inverse(Quaternion const& q) {
        return conjugate(q) / length_squared(q);
    }
} // namespace anton_engine::math

namespace anton_engine {
    inline Quaternion const Quaternion::identity = Quaternion(0, 0, 0, 1);

    inline void Quaternion::conjugate() {
        x = -x;
        y = -y;
        z = -z;
    }

    inline void Quaternion::normalize() {
        float invNorm = math::inv_sqrt(math::length_squared(*this));
        x *= invNorm;
        y *= invNorm;
        z *= invNorm;
        w *= invNorm;
    }

    inline void Quaternion::inverse() {
        conjugate();
        float ns = math::length_squared(*this);
        x /= ns;
        y /= ns;
        z /= ns;
        w /= ns;
    }

    inline Quaternion operator+(Quaternion const& q1, Quaternion const& q2) {
        return {q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w};
    }

    inline Quaternion operator-(Quaternion const& q1, Quaternion const& q2) {
        return {q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w};
    }

    inline Quaternion operator*(Quaternion p, Quaternion q) {
        // clang-format off
        return {p.w * q.x + q.w * p.x + p.y * q.z - p.z * q.y,
                p.w * q.y + q.w * p.y + p.z * q.x - p.x * q.z,
                p.w * q.z + q.w * p.z + p.x * q.y - p.y * q.x,
                p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z};
        // clang-format on
    }

    inline Quaternion operator*(Quaternion const& q, float a) {
        return {q.x * a, q.y * a, q.z * a, q.w * a};
    }

    inline Quaternion operator/(Quaternion const& q, float a) {
        return {q.x / a, q.y / a, q.z / a, q.w / a};
    }
} // namespace anton_engine

#endif // !CORE_MATH_QUATERNION_HPP_INCLUDE
