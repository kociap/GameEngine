#include "math/quaternion.hpp"
#include "math/math.hpp"
#include <cmath>

const Quaternion Quaternion::identity = Quaternion(0, 0, 0, 1);

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

void Quaternion::conjugate() {
    x = -x;
    y = -y;
    z = -z;
}

void Quaternion::normalize() {
    float invNorm = math::inv_sqrt(norm_squared(*this));
    x *= invNorm;
    y *= invNorm;
    z *= invNorm;
    w *= invNorm;
}

void Quaternion::inverse() {
    conjugate();
    float ns = norm_squared(*this);
    x /= ns;
    y /= ns;
    z /= ns;
    w /= ns;
}

Quaternion conjugate(Quaternion const& q) {
    return {-q.x, -q.y, -q.z, q.w};
}

Quaternion normalize(Quaternion const& q) {
    return q * math::inv_sqrt(norm_squared(q));
}

Quaternion inverse(Quaternion const& q) {
    return conjugate(q) / norm_squared(q);
}

float norm_squared(Quaternion const& q) {
    return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float norm(Quaternion const& q) {
    return sqrt(norm_squared(q));
}

Quaternion operator+(Quaternion const& q1, Quaternion const& q2) {
    return {q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w};
}

Quaternion operator-(Quaternion const& q1, Quaternion const& q2) {
    return {q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w};
}

// clang-format off
Quaternion operator*(Quaternion const& p, Quaternion const& q) {
    return {p.w * q.x + q.w * p.x + p.y * q.z - p.z * q.y, 
            p.w * q.y + q.w * p.y + p.z * q.x - p.x * q.z, 
            p.w * q.z + q.w * p.z + p.x * q.y - p.y * q.x,
            p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z};
}
// clang-format on

Quaternion operator*(Quaternion const& q, float a) {
    return {q.x * a, q.y * a, q.z * a, q.w * a};
}

Quaternion operator/(Quaternion const& q, float a) {
    return {q.x / a, q.y / a, q.z / a, q.w / a};
}