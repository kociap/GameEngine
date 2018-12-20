#include "math/matrix4.hpp"
#include <utility>

const Matrix4 Matrix4::zero = Matrix4();
const Matrix4 Matrix4::identity = Matrix4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

Matrix4::Matrix4() : components{} {}

Matrix4::Matrix4(Vector4 const& a, Vector4 const& b, Vector4 const& c, Vector4 const& d)
    : components{
          a.x, a.y, a.z, a.w, b.x, b.y, b.z, b.w, c.x, c.y, c.z, c.w, d.x, d.y, d.z, d.w,
      } {}

float& Matrix4::operator()(int row, int column) {
    return components[row * 4 + column];
}

float const& Matrix4::operator()(int row, int column) const {
    return components[row * 4 + column];
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
    std::swap((*this)(0, 1), (*this)(1, 0));
    std::swap((*this)(0, 2), (*this)(2, 0));
    std::swap((*this)(0, 3), (*this)(3, 0));
    std::swap((*this)(1, 2), (*this)(2, 1));
    std::swap((*this)(1, 3), (*this)(3, 1));
    std::swap((*this)(2, 3), (*this)(3, 2));
    return *this;
}

float const* Matrix4::get_raw() const {
    return components;
}

Matrix4 operator+(Matrix4 const& a, float b) {
    return {{a(0, 0) + b, a(0, 1) + b, a(0, 2) + b, a(0, 3) + b},
            {a(1, 0) + b, a(1, 1) + b, a(1, 2) + b, a(1, 3) + b},
            {a(2, 0) + b, a(2, 1) + b, a(2, 2) + b, a(2, 3) + b},
            {a(3, 0) + b, a(3, 1) + b, a(3, 2) + b, a(3, 3) + b}};
}

Matrix4 operator-(Matrix4 const& a, float b) {
    return {{a(0, 0) - b, a(0, 1) - b, a(0, 2) - b, a(0, 3) - b},
            {a(1, 0) - b, a(1, 1) - b, a(1, 2) - b, a(1, 3) - b},
            {a(2, 0) - b, a(2, 1) - b, a(2, 2) - b, a(2, 3) - b},
            {a(3, 0) - b, a(3, 1) - b, a(3, 2) - b, a(3, 3) - b}};
}

Matrix4 operator*(Matrix4 const& a, float b) {
    return {{a(0, 0) * b, a(0, 1) * b, a(0, 2) * b, a(0, 3) * b},
            {a(1, 0) * b, a(1, 1) * b, a(1, 2) * b, a(1, 3) * b},
            {a(2, 0) * b, a(2, 1) * b, a(2, 2) * b, a(2, 3) * b},
            {a(3, 0) * b, a(3, 1) * b, a(3, 2) * b, a(3, 3) * b}};
}

Matrix4 operator/(Matrix4 const& a, float b) {
    return {{a(0, 0) / b, a(0, 1) / b, a(0, 2) / b, a(0, 3) / b},
            {a(1, 0) / b, a(1, 1) / b, a(1, 2) / b, a(1, 3) / b},
            {a(2, 0) / b, a(2, 1) / b, a(2, 2) / b, a(2, 3) / b},
            {a(3, 0) / b, a(3, 1) / b, a(3, 2) / b, a(3, 3) / b}};
}

Matrix4 operator+(Matrix4 const& a, Matrix4 const& b) {
    return {{a(0, 0) + b(0, 0), a(0, 1) + b(0, 1), a(0, 2) + b(0, 2), a(0, 3) + b(0, 3)},
            {a(1, 0) + b(1, 0), a(1, 1) + b(1, 1), a(1, 2) + b(1, 2), a(1, 3) + b(1, 3)},
            {a(2, 0) + b(2, 0), a(2, 1) + b(2, 1), a(2, 2) + b(2, 2), a(2, 3) + b(2, 3)},
            {a(3, 0) + b(3, 0), a(3, 1) + b(3, 1), a(3, 2) + b(3, 2), a(3, 3) + b(3, 3)}};
}

Matrix4 operator-(Matrix4 const& a, Matrix4 const& b) {
    return {{a(0, 0) - b(0, 0), a(0, 1) - b(0, 1), a(0, 2) - b(0, 2), a(0, 3) - b(0, 3)},
            {a(1, 0) - b(1, 0), a(1, 1) - b(1, 1), a(1, 2) - b(1, 2), a(1, 3) - b(1, 3)},
            {a(2, 0) - b(2, 0), a(2, 1) - b(2, 1), a(2, 2) - b(2, 2), a(2, 3) - b(2, 3)},
            {a(3, 0) - b(3, 0), a(3, 1) - b(3, 1), a(3, 2) - b(3, 2), a(3, 3) - b(3, 3)}};
}

static float multiply_row_column(Matrix4 const& a, int row, Matrix4 const& b, int column) {
    float result = 0;
    for (int i = 0; i < 4; ++i) {
        result += a(row, i) * b(i, column);
    }
    return result;
}

static float multiply_row_column(Vector4 const& a, Matrix4 const& b, int column) {
    float result = 0;
    for (int i = 0; i < 4; ++i) {
        result += a.component(i) * b(i, column);
    }
    return result;
}

Matrix4 operator*(Matrix4 const& lhs, Matrix4 const& rhs) {
    Matrix4 mat;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            mat(i, j) = multiply_row_column(lhs, i, rhs, j);
        }
    }
    return mat;
}

Vector4 operator*(Vector4 const& lhs, Matrix4 const& rhs) {
    return {multiply_row_column(lhs, rhs, 0), multiply_row_column(lhs, rhs, 1), multiply_row_column(lhs, rhs, 2), multiply_row_column(lhs, rhs, 3)};
}

Matrix4 transpose(Matrix4 mat) {
    mat.transpose();
    return mat;
}