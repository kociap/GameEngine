#include "core/math/matrix4.hpp"

float& Matrix4::operator()(int row, int column) {
    return components[column * 4 + row];
}

float const& Matrix4::operator()(int row, int column) const {
    return components[column * 4 + row];
}

Matrix4& Matrix4::operator/=(float num) {
    for (int i = 0; i < 16; ++i) {
        components[i] /= num;
    }
}