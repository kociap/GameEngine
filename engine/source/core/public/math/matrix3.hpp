#ifndef GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE

#include "vector3.hpp"

// Row major
class Matrix3 {
public:
    static Matrix3 const zero;
    static Matrix3 const identity;

    Matrix3();
    Matrix3(Vector3, Vector3, Vector3);

    float& operator()(int row, int column);
    float operator()(int row, int column) const;

    Matrix3& operator+=(float);
    Matrix3& operator-=(float);
    Matrix3& operator*=(float);
    Matrix3& operator/=(float);

    Matrix3& transpose();

    float const* get_raw() const;

private:
    float components[9];
};

Matrix3 operator+(Matrix3, float);
Matrix3 operator-(Matrix3, float);
Matrix3 operator*(Matrix3, float);
Matrix3 operator/(Matrix3, float);
Matrix3 operator+(Matrix3, Matrix3);
Matrix3 operator-(Matrix3, Matrix3);
Matrix3 operator*(Matrix3, Matrix3);
Vector3 operator*(Vector3, Matrix3);

namespace math {
    float determinant(Matrix3);
    Matrix3 adjugate(Matrix3);
    Matrix3 inverse(Matrix3);
} // namespace math

#endif // !GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
