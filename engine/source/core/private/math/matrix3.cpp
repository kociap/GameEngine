#include <anton_stl/utility.hpp>
#include <math/matrix3.hpp>

Matrix3 const Matrix3::zero = Matrix3();
Matrix3 const Matrix3::identity = Matrix3({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

Matrix3::Matrix3(): components{} {}
Matrix3::Matrix3(Vector3 a, Vector3 b, Vector3 c): components{a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z} {}

float& Matrix3::operator()(int row, int column) {
    return components[row * 3 + column];
}

float Matrix3::operator()(int row, int column) const {
    return components[row * 3 + column];
}

Matrix3& Matrix3::operator+=(float a) {
    for (int i = 0; i < 9; ++i) {
        components[i] += a;
    }
    return *this;
}

Matrix3& Matrix3::operator-=(float a) {
    for (int i = 0; i < 9; ++i) {
        components[i] -= a;
    }
    return *this;
}

Matrix3& Matrix3::operator*=(float num) {
    for (int i = 0; i < 9; ++i) {
        components[i] *= num;
    }
    return *this;
}

Matrix3& Matrix3::operator/=(float num) {
    for (int i = 0; i < 9; ++i) {
        components[i] /= num;
    }
    return *this;
}

Matrix3& Matrix3::transpose() {
    anton_stl::swap((*this)(0, 1), (*this)(1, 0));
    anton_stl::swap((*this)(0, 2), (*this)(2, 0));
    anton_stl::swap((*this)(1, 2), (*this)(2, 1));
    return *this;
}

float const* Matrix3::get_raw() const {
    return components;
}

Matrix3 operator+(Matrix3 m, float a) {
    m += a;
    return m;
}

Matrix3 operator-(Matrix3 m, float a) {
    m -= a;
    return m;
}

Matrix3 operator*(Matrix3 m, float a) {
    m *= a;
    return m;
}

Matrix3 operator/(Matrix3 m, float a) {
    m /= a;
    return m;
}

Matrix3 operator+(Matrix3 a, Matrix3 b) {
    return {{a(0, 0) + b(0, 0), a(0, 1) + b(0, 1), a(0, 2) + b(0, 2)},
            {a(1, 0) + b(1, 0), a(1, 1) + b(1, 1), a(1, 2) + b(1, 2)},
            {a(2, 0) + b(2, 0), a(2, 1) + b(2, 1), a(2, 2) + b(2, 2)}};
}

Matrix3 operator-(Matrix3 a, Matrix3 b) {
    return {{a(0, 0) - b(0, 0), a(0, 1) - b(0, 1), a(0, 2) - b(0, 2)},
            {a(1, 0) - b(1, 0), a(1, 1) - b(1, 1), a(1, 2) - b(1, 2)},
            {a(2, 0) - b(2, 0), a(2, 1) - b(2, 1), a(2, 2) - b(2, 2)}};
}

float multiply_row_column(Matrix3 lhs, int row, Matrix3 rhs, int column) {
    float result = 0;
    for (int i = 0; i < 3; ++i) {
        result += lhs(row, i) * rhs(i, column);
    }
    return result;
}

float multiply_row_column(Vector3 a, Matrix3 b, int column) {
    float result = 0;
    for (int i = 0; i < 3; ++i) {
        result += a.component(i) * b(i, column);
    }
    return result;
}

Matrix3 operator*(Matrix3 lhs, Matrix3 rhs) {
    Matrix3 mat;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            mat(i, j) = multiply_row_column(lhs, i, rhs, j);
        }
    }
    return mat;
}

Vector3 operator*(Vector3 lhs, Matrix3 rhs) {
    return {multiply_row_column(lhs, rhs, 0), multiply_row_column(lhs, rhs, 1), multiply_row_column(lhs, rhs, 2)};
}

namespace math {
    static float determinant2x2(float m00, float m01, float m10, float m11) {
        return m00 * m11 - m01 * m10;
    }

    float determinant(Matrix3 m) {
        // clang-format off
        return   m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1))
               - m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0))
               + m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
        // clang-format on
    }

    Matrix3 adjugate(Matrix3 m) {
        float m00 = determinant2x2(m(1, 1), m(1, 2), m(2, 1), m(2, 2));
        float m01 = determinant2x2(m(1, 0), m(1, 2), m(2, 0), m(2, 2));
        float m02 = determinant2x2(m(1, 0), m(1, 1), m(2, 0), m(2, 1));

        float m10 = determinant2x2(m(0, 1), m(0, 2), m(2, 1), m(2, 2));
        float m11 = determinant2x2(m(0, 0), m(0, 2), m(2, 0), m(2, 2));
        float m12 = determinant2x2(m(0, 0), m(0, 1), m(2, 0), m(2, 1));

        float m20 = determinant2x2(m(0, 1), m(0, 2), m(1, 1), m(1, 2));
        float m21 = determinant2x2(m(0, 0), m(0, 2), m(1, 0), m(1, 2));
        float m22 = determinant2x2(m(0, 0), m(0, 1), m(1, 0), m(1, 1));
        // clang-format off
        return Matrix3(
            {m00, -m10, m20}, 
            {-m01, m11, -m21}, 
            {m02, -m12, m22});
        // clang-format on
    }

    Matrix3 inverse(Matrix3 m) {
        return adjugate(m) / determinant(m);
    }
} // namespace math
