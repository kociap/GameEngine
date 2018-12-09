#include "math/vector3.hpp"

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

float& Vector3::component(int index) {
    return (&x)[index];
}

float const& Vector3::component(int index) const {
    return (&x)[index];
}

float Vector3::dot(Vector3 const& vec1, Vector3 const& vec2) {
    return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}