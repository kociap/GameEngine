#include "math/matrix3.hpp"

const Matrix3 Matrix3::zero = Matrix3();
const Matrix3 Matrix3::identity = Matrix3({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

Matrix3::Matrix3() : components{} {}
Matrix3::Matrix3(Vector3 const& a, Vector3 const& b, Vector3 const& c) : components{a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z} {}

float& Matrix3::operator()(int row, int column) {
    return components[row * 3 + column];
}

float const& Matrix3::operator()(int row, int column) const {
    return components[row * 3 + column];
}

Matrix3 operator+(Matrix3 const& a, float b) {
    return {{a(0, 0) + b, a(0, 1) + b, a(0, 2) + b}, {a(1, 0) + b, a(1, 1) + b, a(1, 2) + b}, {a(2, 0) + b, a(2, 1) + b, a(2, 2) + b}};
}

Matrix3 operator-(Matrix3 const& a, float b) {
    return {{a(0, 0) - b, a(0, 1) - b, a(0, 2) - b}, {a(1, 0) - b, a(1, 1) - b, a(1, 2) - b}, {a(2, 0) - b, a(2, 1) - b, a(2, 2) - b}};
}

Matrix3 operator*(Matrix3 const& a, float b) {
    return {{a(0, 0) * b, a(0, 1) * b, a(0, 2) * b}, {a(1, 0) * b, a(1, 1) * b, a(1, 2) * b}, {a(2, 0) * b, a(2, 1) * b, a(2, 2) * b}};
}

Matrix3 operator/(Matrix3 const& a, float b) {
    return {{a(0, 0) / b, a(0, 1) / b, a(0, 2) / b}, {a(1, 0) / b, a(1, 1) / b, a(1, 2) / b}, {a(2, 0) / b, a(2, 1) / b, a(2, 2) / b}};
}

Matrix3 operator+(Matrix3 const& a, Matrix3 const& b) {
    return {{a(0, 0) + b(0, 0), a(0, 1) + b(0, 1), a(0, 2) + b(0, 2)},
            {a(1, 0) + b(1, 0), a(1, 1) + b(1, 1), a(1, 2) + b(1, 2)},
            {a(2, 0) + b(2, 0), a(2, 1) + b(2, 1), a(2, 2) + b(2, 2)}};
}

Matrix3 operator-(Matrix3 const& a, Matrix3 const& b) {
    return {{a(0, 0) - b(0, 0), a(0, 1) - b(0, 1), a(0, 2) - b(0, 2)},
            {a(1, 0) - b(1, 0), a(1, 1) - b(1, 1), a(1, 2) - b(1, 2)},
            {a(2, 0) - b(2, 0), a(2, 1) - b(2, 1), a(2, 2) - b(2, 2)}};
}

float multiply_row_column(Matrix3 const& lhs, int row, Matrix3 const& rhs, int column) {
    float result = 0;
    for (int i = 0; i < 3; ++i) {
        result += lhs(row, i) * rhs(i, column);
    }
    return result;
}

float multiply_row_column(Vector3 const& a, Matrix3 const& b, int column) {
    float result = 0;
    for (int i = 0; i < 3; ++i) {
        result += a.component(i) * b(i, column);
    }
    return result;
}

Matrix3 operator*(Matrix3 const& lhs, Matrix3 const& rhs) {
    Matrix3 mat;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            mat(i, j) = multiply_row_column(lhs, i, rhs, j);
        }
    }
    return mat;
}

Vector3 operator*(Vector3 const& lhs, Matrix3 const& rhs) {
    return {multiply_row_column(lhs, rhs, 0), multiply_row_column(lhs, rhs, 1), multiply_row_column(lhs, rhs, 2)};
}

float determinant(Matrix3 const& m) {
    return m(0, 0) * m(1, 1) * m(2, 2) + m(1, 0) * m(2, 1) * m(0, 2) + m(2, 0) * m(0, 1) * m(1, 2) - m(0, 0) * m(2, 1) * m(1, 2) - m(1, 0) * m(0, 1) * m(2, 2) -
           m(2, 0) * m(1, 1) * m(0, 2);
}

//Matrix3 adjoint(Matrix3 const& m) {}

//Matrix3 inverse(Matrix3 const& m) {
//    return adjoint(m) / determinant(m);
//}