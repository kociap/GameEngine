#ifndef GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE

#include <math/vector2.hpp>

// Row major
class Matrix2 {
public:
    static ENGINE_API Matrix2 const zero;
    static ENGINE_API Matrix2 const identity;

    Matrix2();
    Matrix2(Vector2, Vector2);
    Matrix2(float m00, float m01, float m10, float m11);

    float& operator()(int row, int column);
    float operator()(int row, int column) const;

    Matrix2& operator+=(float);
    Matrix2& operator-=(float);
    Matrix2& operator*=(float);
    Matrix2& operator/=(float);

    Matrix2& transpose();

    float const* get_raw() const;

private:
    float components[4];
};

Matrix2 operator+(Matrix2, float);
Matrix2 operator-(Matrix2, float);
Matrix2 operator*(Matrix2, float);
Matrix2 operator/(Matrix2, float);
Matrix2 operator+(Matrix2, Matrix2);
Matrix2 operator-(Matrix2, Matrix2);
Matrix2 operator*(Matrix2, Matrix2);
Vector2 operator*(Vector2, Matrix2);

namespace math {
    Matrix2 transpose(Matrix2);
    float determinant(Matrix2);
    Matrix2 adjugate(Matrix2);
    Matrix2 inverse(Matrix2);
} // namespace math

#endif // !GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE
