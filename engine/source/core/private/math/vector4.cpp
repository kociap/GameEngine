#include "math/vector4.hpp"

Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

float& Vector4::component(int index) {
    return (&x)[index];
}

float const& Vector4::component(int index) const {
    return (&x)[index];
}