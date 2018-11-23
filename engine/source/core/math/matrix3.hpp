#ifndef GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE

#include "../coremacrodefinitions.hpp"
#include "vector3.hpp"

class Matrix3 {
public:
    static const Matrix3 identity;

    //Matrix3(Vector3 const& a, Vector3 const& b, Vector3 const& c): components{a(0), a(1), a(2)} {}

    float& operator()(int column, int row) {
        return components[column * 3 + row];
    }

    float const& operator()(int column, int row) const {
        return components[column * 3 + row];
    }

    Matrix3& operator/=(float num) {
        for (int i = 0; i < 9; ++i) {
            components[i] /= num;
        }
    }

    friend Matrix3 operator/(Matrix3 const&, float);

private:
    float components[9];
};

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

#endif // !GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
