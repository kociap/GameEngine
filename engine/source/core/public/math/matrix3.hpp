#ifndef GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE

#include "../coremacrodefinitions.hpp"
#include "vector3.hpp"

class Matrix3 {
public:
    static const Matrix3 identity;

    //Matrix3(Vector3 const& a, Vector3 const& b, Vector3 const& c): components{a(0), a(1), a(2)} {}

    float& operator()(int row, int column);
    float const& operator()(int row, int column) const;

    Matrix3& operator*(float);
    Matrix3& operator/=(float);

    friend Matrix3 operator/(Matrix3 const&, float);

private:
    float components[9];
};

float determinant(Matrix3 const& m);
Matrix3 adjoint(Matrix3 const& m);
Matrix3 inverse(Matrix3 const& m);

#endif // !GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
