#ifndef GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE

#include "vector4.hpp"
#include <cstdint>

// Row major
class Matrix4 {
public:
    static Matrix4 const zero;
    static Matrix4 const identity;

    Matrix4();
    Matrix4(Vector4, Vector4, Vector4, Vector4);

    float& operator()(int row, int column);
    float operator()(int row, int column) const;

    Matrix4& operator+=(float);
    Matrix4& operator-=(float);
    Matrix4& operator*=(float);
    Matrix4& operator/=(float);

    Matrix4& transpose();

    float const* get_raw() const;

private:
    float components[16];
};

Matrix4 operator+(Matrix4, float);
Matrix4 operator-(Matrix4, float);
Matrix4 operator*(Matrix4, float);
Matrix4 operator/(Matrix4, float);
Matrix4 operator+(Matrix4, Matrix4);
Matrix4 operator-(Matrix4, Matrix4);
Matrix4 operator*(Matrix4, Matrix4);
Vector4 operator*(Vector4, Matrix4);

namespace math {
    Matrix4 transpose(Matrix4);
    float determinant(Matrix4);
    Matrix4 adjugate(Matrix4);
    Matrix4 inverse(Matrix4);
} // namespace math

#endif // !GAME_ENGINE_CORE_MATH_MARTIX4_HPP_INCLUDE
