#include "math/quaternion.hpp"
#include "math/math.hpp"

const Quaternion Quaternion::identity = Quaternion(0, 0, 0, 1);

Quaternion::Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

void Quaternion::conjugate() {
    x = -x;
    y = -y;
    z = -z;
}

void Quaternion::normalize() {
    float invNorm = inv_sqrt(norm_squared(*this));
    x *= invNorm;
    y *= invNorm;
    z *= invNorm;
    w *= invNorm;
}

void Quaternion::invert() {
    conjugate();
    *this /= norm_squared(*this);
}

Quaternion& Quaternion::operator+=(Quaternion const& q) {
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
    return *this;
}

Quaternion& Quaternion::operator-=(Quaternion const& q) {
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
    return *this;
}

Quaternion& Quaternion::operator*=(Quaternion const&) {
    return *this;
}

Quaternion& Quaternion::operator*=(float a) {
    x *= a;
    y *= a;
    z *= a;
    w *= a;
    return *this;
}

Quaternion& Quaternion::operator/=(float a) {
    x /= a;
    y /= a;
    z /= a;
    w /= a;
    return *this;
}

Quaternion conjugate(Quaternion const& q) {
    Quaternion qnion(q);
    qnion.conjugate();
    return qnion;
}

Quaternion normalized(Quaternion const& q) {
    return q * inv_sqrt(norm_squared(q));
}

Quaternion inverse(Quaternion const& q) {
    Quaternion qnion(conjugate(q));
    qnion /= norm_squared(q);
    return qnion;
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

//Quaternion operator*(Quaternion const&, Quaternion const&) {
//    return {};
//}

Quaternion operator*(Quaternion const& q, float a) {
    return {q.x * a, q.y * a, q.z * a, q.w * a};
}

Quaternion operator/(Quaternion const& q, float a) {
    return {q.x / a, q.y / a, q.z / a, q.w / a};
}