#ifndef GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE

#include "../coremacrodefinitions.hpp"
#include "vector3.hpp"

// Row major
class Matrix3 {
public:
    static const Matrix3 zero;
    static const Matrix3 identity;

    Matrix3();
    Matrix3(Vector3 const&, Vector3 const&, Vector3 const&);

    float& operator()(int row, int column);
    float const& operator()(int row, int column) const;

private:
    float components[9];
};

Matrix3 operator+(Matrix3 const&, float);
Matrix3 operator-(Matrix3 const&, float);
Matrix3 operator*(Matrix3 const&, float);
Matrix3 operator/(Matrix3 const&, float);
Matrix3 operator+(Matrix3 const&, Matrix3 const&);
Matrix3 operator-(Matrix3 const&, Matrix3 const&);
Matrix3 operator*(Matrix3 const&, Matrix3 const&);
Vector3 operator*(Vector3 const&, Matrix3 const&);

float determinant(Matrix3 const& m);
Matrix3 adjoint(Matrix3 const& m);
Matrix3 inverse(Matrix3 const& m);

#endif // !GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
