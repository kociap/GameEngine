#include <anton_stl/utility.hpp>
#include <math/matrix2.hpp>

Matrix2 const zero = Matrix2();
Matrix2 const Matrix2::identity = Matrix2({1, 0}, {0, 1});

Matrix2::Matrix2(): components{} {}
Matrix2::Matrix2(Vector2 v1, Vector2 v2): components{v1.x, v1.y, v2.x, v2.y} {}
Matrix2::Matrix2(float m00, float m01, float m10, float m11): components{m00, m01, m10, m11} {}

float& Matrix2::operator()(int row, int column) {
    return components[row * 2 + column];
}

float Matrix2::operator()(int row, int column) const {
    return components[row * 2 + column];
}

Matrix2& Matrix2::operator+=(float a) {
    for (int i = 0; i < 4; ++i) {
        components[i] += a;
    }
    return *this;
}

Matrix2& Matrix2::operator-=(float a) {
    for (int i = 0; i < 4; ++i) {
        components[i] -= a;
    }
    return *this;
}

Matrix2& Matrix2::operator*=(float a) {
    for (int i = 0; i < 4; ++i) {
        components[i] *= a;
    }
    return *this;
}

Matrix2& Matrix2::operator/=(float a) {
    for (int i = 0; i < 4; ++i) {
        components[i] /= a;
    }
    return *this;
}

Matrix2& Matrix2::transpose() {
    anton_stl::swap((*this)(1, 0), (*this)(0, 1));
    return *this;
}

float const* Matrix2::get_raw() const {
    return components;
}

Matrix2 operator+(Matrix2 m, float a) {
    m += a;
    return m;
}

Matrix2 operator-(Matrix2 m, float a) {
    m -= a;
    return m;
}

Matrix2 operator*(Matrix2 m, float a) {
    m *= a;
    return m;
}

Matrix2 operator/(Matrix2 m, float a) {
    m /= a;
    return m;
}

Matrix2 operator+(Matrix2 m1, Matrix2 m2) {
    m1(0, 0) += m2(0, 0);
    m1(0, 1) += m2(0, 1);
    m1(1, 0) += m2(1, 0);
    m1(1, 1) += m2(1, 1);
    return m1;
}

Matrix2 operator-(Matrix2 m1, Matrix2 m2) {
    m1(0, 0) -= m2(0, 0);
    m1(0, 1) -= m2(0, 1);
    m1(1, 0) -= m2(1, 0);
    m1(1, 1) -= m2(1, 1);
    return m1;
}

Matrix2 operator*(Matrix2 m1, Matrix2 m2) {
    return {{m1(0, 0) * m2(0, 0) + m1(0, 1) * m2(1, 0), m1(0, 0) * m2(0, 1) + m1(0, 1) * m2(1, 1)},
            {m1(1, 0) * m2(0, 0) + m1(1, 1) * m2(1, 0), m1(1, 0) * m2(0, 1) + m1(1, 1) * m2(1, 1)}};
}

Vector2 operator*(Vector2 v, Matrix2 m) {
    return {v[0] * m(0, 0) + v[1] * m(1, 0), v[0] * m(0, 1) + v[1] * m(1, 1)};
}

namespace math {
    Matrix2 transpose(Matrix2 m) {
        m.transpose();
        return m;
    }

    float determinant(Matrix2 m) {
        return m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1);
    }

    Matrix2 adjugate(Matrix2 m) {
        return {{m(1, 1), -m(0, 1)}, {-m(1, 0), m(0, 0)}};
    }

    Matrix2 inverse(Matrix2 m) {
        return adjugate(m) / determinant(m);
    }
} // namespace math
