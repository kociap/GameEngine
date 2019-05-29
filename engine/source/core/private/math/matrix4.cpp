#include "math/matrix4.hpp"
#include "utility.hpp"

const Matrix4 Matrix4::zero = Matrix4();
const Matrix4 Matrix4::identity = Matrix4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

Matrix4::Matrix4(): components{} {}

Matrix4::Matrix4(Vector4 a, Vector4 b, Vector4 c, Vector4 d)
    : components{
          a.x, a.y, a.z, a.w, b.x, b.y, b.z, b.w, c.x, c.y, c.z, c.w, d.x, d.y, d.z, d.w,
      } {}

float& Matrix4::operator()(int row, int column) {
    return components[row * 4 + column];
}

float Matrix4::operator()(int row, int column) const {
    return components[row * 4 + column];
}

Matrix4& Matrix4::operator+=(float a) {
    for (int i = 0; i < 16; ++i) {
        components[i] += a;
    }
    return *this;
}

Matrix4& Matrix4::operator-=(float a) {
    for (int i = 0; i < 16; ++i) {
        components[i] -= a;
    }
    return *this;
}

Matrix4& Matrix4::operator*=(float num) {
    for (int i = 0; i < 16; ++i) {
        components[i] *= num;
    }
    return *this;
}

Matrix4& Matrix4::operator/=(float num) {
    for (int i = 0; i < 16; ++i) {
        components[i] /= num;
    }
    return *this;
}

Matrix4& Matrix4::transpose() {
    swap((*this)(0, 1), (*this)(1, 0));
    swap((*this)(0, 2), (*this)(2, 0));
    swap((*this)(0, 3), (*this)(3, 0));
    swap((*this)(1, 2), (*this)(2, 1));
    swap((*this)(1, 3), (*this)(3, 1));
    swap((*this)(2, 3), (*this)(3, 2));
    return *this;
}

float const* Matrix4::get_raw() const {
    return components;
}

Matrix4 operator+(Matrix4 m, float a) {
    m += a;
    return m;
}

Matrix4 operator-(Matrix4 m, float a) {
    m -= a;
    return m;
}

Matrix4 operator*(Matrix4 m, float a) {
    m *= a;
    return m;
}

Matrix4 operator/(Matrix4 m, float a) {
    m /= a;
    return m;
}

Matrix4 operator+(Matrix4 a, Matrix4 b) {
    return {{a(0, 0) + b(0, 0), a(0, 1) + b(0, 1), a(0, 2) + b(0, 2), a(0, 3) + b(0, 3)},
            {a(1, 0) + b(1, 0), a(1, 1) + b(1, 1), a(1, 2) + b(1, 2), a(1, 3) + b(1, 3)},
            {a(2, 0) + b(2, 0), a(2, 1) + b(2, 1), a(2, 2) + b(2, 2), a(2, 3) + b(2, 3)},
            {a(3, 0) + b(3, 0), a(3, 1) + b(3, 1), a(3, 2) + b(3, 2), a(3, 3) + b(3, 3)}};
}

Matrix4 operator-(Matrix4 a, Matrix4 b) {
    return {{a(0, 0) - b(0, 0), a(0, 1) - b(0, 1), a(0, 2) - b(0, 2), a(0, 3) - b(0, 3)},
            {a(1, 0) - b(1, 0), a(1, 1) - b(1, 1), a(1, 2) - b(1, 2), a(1, 3) - b(1, 3)},
            {a(2, 0) - b(2, 0), a(2, 1) - b(2, 1), a(2, 2) - b(2, 2), a(2, 3) - b(2, 3)},
            {a(3, 0) - b(3, 0), a(3, 1) - b(3, 1), a(3, 2) - b(3, 2), a(3, 3) - b(3, 3)}};
}

static float multiply_row_column(Matrix4 a, int row, Matrix4 b, int column) {
    float result = 0;
    for (int i = 0; i < 4; ++i) {
        result += a(row, i) * b(i, column);
    }
    return result;
}

static float multiply_row_column(Vector4 a, Matrix4 b, int column) {
    float result = 0;
    for (int i = 0; i < 4; ++i) {
        result += a.component(i) * b(i, column);
    }
    return result;
}

Matrix4 operator*(Matrix4 lhs, Matrix4 rhs) {
    Matrix4 mat;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat(i, j) = multiply_row_column(lhs, i, rhs, j);
        }
    }
    return mat;
}

Vector4 operator*(Vector4 lhs, Matrix4 rhs) {
    return {multiply_row_column(lhs, rhs, 0), multiply_row_column(lhs, rhs, 1), multiply_row_column(lhs, rhs, 2), multiply_row_column(lhs, rhs, 3)};
}

namespace math {
    Matrix4 transpose(Matrix4 mat) {
        mat.transpose();
        return mat;
    }

    static float determinant3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21;
    }

    float determinant(Matrix4 m) {
        float det0 = determinant3x3(m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3));
        float det1 = determinant3x3(m(1, 0), m(1, 2), m(1, 3), m(2, 0), m(2, 2), m(2, 3), m(3, 0), m(3, 2), m(3, 3));
        float det2 = determinant3x3(m(1, 0), m(1, 1), m(1, 3), m(2, 0), m(2, 1), m(2, 3), m(3, 0), m(3, 1), m(3, 3));
        float det3 = determinant3x3(m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2));
        return m(0, 0) * det0 - m(0, 1) * det1 + m(0, 2) * det2 - m(0, 3) * det3;
    }

    Matrix4 adjugate(Matrix4 m) {
        float m00 = determinant3x3(m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3));
        float m01 = determinant3x3(m(1, 0), m(1, 2), m(1, 3), m(2, 0), m(2, 2), m(2, 3), m(3, 0), m(3, 2), m(3, 3));
        float m02 = determinant3x3(m(1, 0), m(1, 1), m(1, 3), m(2, 0), m(2, 1), m(2, 3), m(3, 0), m(3, 1), m(3, 3));
        float m03 = determinant3x3(m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2));

        float m10 = determinant3x3(m(0, 1), m(0, 2), m(0, 3), m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3));
        float m11 = determinant3x3(m(0, 0), m(0, 2), m(0, 3), m(2, 0), m(2, 2), m(2, 3), m(3, 0), m(3, 2), m(3, 3));
        float m12 = determinant3x3(m(0, 0), m(0, 1), m(0, 3), m(2, 0), m(2, 1), m(2, 3), m(3, 0), m(3, 1), m(3, 3));
        float m13 = determinant3x3(m(0, 0), m(0, 1), m(0, 2), m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2));

        float m20 = determinant3x3(m(0, 1), m(0, 2), m(0, 3), m(1, 1), m(1, 2), m(1, 3), m(3, 1), m(3, 2), m(3, 3));
        float m21 = determinant3x3(m(0, 0), m(0, 2), m(0, 3), m(1, 0), m(1, 2), m(1, 3), m(3, 0), m(3, 2), m(3, 3));
        float m22 = determinant3x3(m(0, 0), m(0, 1), m(0, 3), m(1, 0), m(1, 1), m(1, 3), m(3, 0), m(3, 1), m(3, 3));
        float m23 = determinant3x3(m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2), m(3, 0), m(3, 1), m(3, 2));

        float m30 = determinant3x3(m(0, 1), m(0, 2), m(0, 3), m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3));
        float m31 = determinant3x3(m(0, 0), m(0, 2), m(0, 3), m(1, 0), m(1, 2), m(1, 3), m(2, 0), m(2, 2), m(2, 3));
        float m32 = determinant3x3(m(0, 0), m(0, 1), m(0, 3), m(1, 0), m(1, 1), m(1, 3), m(2, 0), m(2, 1), m(2, 3));
        float m33 = determinant3x3(m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2));
        // clang-format off
        return Matrix4(
            {m00, -m10, m20, -m30}, 
            {-m01, m11, -m21, m31}, 
            {m02, -m12, m22, -m32}, 
            {-m03, m13, -m23, m33});
        // clang-format on
    }

    Matrix4 inverse(Matrix4 m) {
        return adjugate(m) / determinant(m);
    }
} // namespace math