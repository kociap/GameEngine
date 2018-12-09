#ifndef GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE

#include "../coremacrodefinitions.hpp"
#include "vector4.hpp"

// Row major
class Matrix4 {
public:
    static const Matrix4 identity;

	Matrix4();
    Matrix4(Vector4 const&, Vector4 const&, Vector4 const&, Vector4 const&);

    float& operator()(int row, int column);
    float const& operator()(int row, int column) const;

    Matrix4& operator*=(float);
    Matrix4& operator/=(float);

    friend Matrix4 operator*(Matrix4 const&, Matrix4 const&);
    friend Matrix4 operator*(Matrix4 const&, float);
    friend Matrix4 operator/(Matrix4 const&, float);

private:
    float components[16];
};

// float determinant(Matrix4 const& m) {
//     return m(0, 0) * m(1, 1) * m(2, 2) + m(1, 0) * m(2, 1) * m(0, 2) + m(2, 0) * m(0, 1) * m(1, 2) - m(0, 0) * m(2, 1) * m(1, 2) - m(1, 0) * m(0, 1) * m(2, 2) -
//            m(2, 0) * m(1, 1) * m(0, 2);
// }

Matrix4 adjoint(Matrix4 const& m);

Matrix4 inverse(Matrix4 const& m);

#endif // !GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE
