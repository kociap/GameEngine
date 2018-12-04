#include "core/math/matrix3.hpp"

float& Matrix3::operator()(int row, int column) {
    return components[column * 3 + row];
}

float const& Matrix3::operator()(int row, int column) const {
    return components[column * 3 + row];
}

Matrix3& Matrix3::operator*(float num) {}

Matrix3& Matrix3::operator/=(float num) {
    for (int i = 0; i < 9; ++i) {
        components[i] /= num;
    }
}

Matrix3 operator/(Matrix3 const& m, float a) {
    Matrix3 copy(m);
    copy /= a;
    return copy;
}

float determinant(Matrix3 const& m) {
    return m(0, 0) * m(1, 1) * m(2, 2) + m(1, 0) * m(2, 1) * m(0, 2) + m(2, 0) * m(0, 1) * m(1, 2) - m(0, 0) * m(2, 1) * m(1, 2) - m(1, 0) * m(0, 1) * m(2, 2) -
           m(2, 0) * m(1, 1) * m(0, 2);
}

Matrix3 adjoint(Matrix3 const& m) {}

Matrix3 inverse(Matrix3 const& m) {
    return adjoint(m) / determinant(m);
}