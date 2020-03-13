#ifndef CORE_MATH_MARTIX4_HPP_INCLUDE
#define CORE_MATH_MARTIX4_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <cstdint>
#include <core/math/vector4.hpp>

namespace anton_engine {
    // Row major
    class Matrix4 {
    public:
        static Matrix4 const zero;
        static Matrix4 const identity;

        Matrix4();
        Matrix4(Vector4, Vector4, Vector4, Vector4);

        Vector4& operator[](int row);
        Vector4 operator[](int row) const;
        float& operator()(int row, int column);
        float operator()(int row, int column) const;
        float const* get_raw() const;

        Matrix4& operator+=(float);
        Matrix4& operator-=(float);
        Matrix4& operator*=(float);
        Matrix4& operator/=(float);

        Matrix4& transpose();

    private:
        Vector4 rows[4];
    };

    Matrix4 operator+(Matrix4, float);
    Matrix4 operator-(Matrix4, float);
    Matrix4 operator*(Matrix4, float);
    Matrix4 operator/(Matrix4, float);
    Matrix4 operator+(Matrix4, Matrix4);
    Matrix4 operator-(Matrix4, Matrix4);
    Matrix4 operator*(Matrix4, Matrix4);
    Vector4 operator*(Vector4, Matrix4);

} // namespace anton_engine

namespace anton_engine::math {
    Matrix4 transpose(Matrix4);
    float determinant(Matrix4);
    Matrix4 adjugate(Matrix4);
    Matrix4 inverse(Matrix4);
} // namespace anton_engine::math

namespace anton_engine {
    inline Matrix4 const Matrix4::zero = Matrix4();
    inline Matrix4 const Matrix4::identity = Matrix4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

    inline Matrix4::Matrix4(): rows{} {}

    inline Matrix4::Matrix4(Vector4 const a, Vector4 const b, Vector4 const c, Vector4 const d): rows{a, b, c, d} {}

    inline Vector4& Matrix4::operator[](int const row) {
        return rows[row];
    }

    inline Vector4 Matrix4::operator[](int const row) const {
        return rows[row];
    }

    inline float& Matrix4::operator()(int const row, int const column) {
        return rows[row][column];
    }

    inline float Matrix4::operator()(int const row, int const column) const {
        return rows[row][column];
    }

    inline float const* Matrix4::get_raw() const {
        return reinterpret_cast<float const*>(rows);
    }

    inline Matrix4& Matrix4::operator+=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator-=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator*=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] *= a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator/=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] /= a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::transpose() {
        atl::swap((*this)[0][1], (*this)[1][0]);
        atl::swap((*this)[0][2], (*this)[2][0]);
        atl::swap((*this)[0][3], (*this)[3][0]);
        atl::swap((*this)[1][2], (*this)[2][1]);
        atl::swap((*this)[1][3], (*this)[3][1]);
        atl::swap((*this)[2][3], (*this)[3][2]);
        return *this;
    }

    inline Matrix4 operator+(Matrix4 m, float const a) {
        m += a;
        return m;
    }

    inline Matrix4 operator-(Matrix4 m, float const a) {
        m -= a;
        return m;
    }

    inline Matrix4 operator*(Matrix4 m, float const a) {
        m *= a;
        return m;
    }

    inline Matrix4 operator/(Matrix4 m, float const a) {
        m /= a;
        return m;
    }

    inline Matrix4 operator+(Matrix4 const a, Matrix4 const b) {
        return {{a[0][0] + b[0][0], a[0][1] + b[0][1], a[0][2] + b[0][2], a[0][3] + b[0][3]},
                {a[1][0] + b[1][0], a[1][1] + b[1][1], a[1][2] + b[1][2], a[1][3] + b[1][3]},
                {a[2][0] + b[2][0], a[2][1] + b[2][1], a[2][2] + b[2][2], a[2][3] + b[2][3]},
                {a[3][0] + b[3][0], a[3][1] + b[3][1], a[3][2] + b[3][2], a[3][3] + b[3][3]}};
    }

    inline Matrix4 operator-(Matrix4 const a, Matrix4 const b) {
        return {{a[0][0] - b[0][0], a[0][1] - b[0][1], a[0][2] - b[0][2], a[0][3] - b[0][3]},
                {a[1][0] - b[1][0], a[1][1] - b[1][1], a[1][2] - b[1][2], a[1][3] - b[1][3]},
                {a[2][0] - b[2][0], a[2][1] - b[2][1], a[2][2] - b[2][2], a[2][3] - b[2][3]},
                {a[3][0] - b[3][0], a[3][1] - b[3][1], a[3][2] - b[3][2], a[3][3] - b[3][3]}};
    }

    inline Matrix4 operator*(Matrix4 const lhs, Matrix4 const rhs) {
        Matrix4 r;
        for (int i = 0; i < 4; ++i) {
            r[i][0] = lhs[i][0] * rhs[0][0] + lhs[i][1] * rhs[1][0] + lhs[i][2] * rhs[2][0] + lhs[i][3] * rhs[3][0];
            r[i][1] = lhs[i][0] * rhs[0][1] + lhs[i][1] * rhs[1][1] + lhs[i][2] * rhs[2][1] + lhs[i][3] * rhs[3][1];
            r[i][2] = lhs[i][0] * rhs[0][2] + lhs[i][1] * rhs[1][2] + lhs[i][2] * rhs[2][2] + lhs[i][3] * rhs[3][2];
            r[i][3] = lhs[i][0] * rhs[0][3] + lhs[i][1] * rhs[1][3] + lhs[i][2] * rhs[2][3] + lhs[i][3] * rhs[3][3];
        }
        return r;
    }

    inline Vector4 operator*(Vector4 const lhs, Matrix4 const rhs) {
        Vector4 r;
        r[0] = lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0] + lhs[2] * rhs[2][0] + lhs[3] * rhs[3][0];
        r[1] = lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] + lhs[2] * rhs[2][1] + lhs[3] * rhs[3][1];
        r[2] = lhs[0] * rhs[0][2] + lhs[1] * rhs[1][2] + lhs[2] * rhs[2][2] + lhs[3] * rhs[3][2];
        r[3] = lhs[0] * rhs[0][3] + lhs[1] * rhs[1][3] + lhs[2] * rhs[2][3] + lhs[3] * rhs[3][3];
        return r;
    }
} // namespace anton_engine

namespace anton_engine::math {
    inline Matrix4 transpose(Matrix4 mat) {
        mat.transpose();
        return mat;
    }

    inline static float determinant3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
        return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21;
    }

    inline float determinant(Matrix4 const m) {
        float det0 = determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        float det1 = determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        float det2 = determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        float det3 = determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
        return m[0][0] * det0 - m[0][1] * det1 + m[0][2] * det2 - m[0][3] * det3;
    }

    inline Matrix4 adjugate(Matrix4 const m) {
        float m00 = determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        float m01 = determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        float m02 = determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        float m03 = determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

        float m10 = determinant3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        float m11 = determinant3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        float m12 = determinant3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        float m13 = determinant3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

        float m20 = determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
        float m21 = determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
        float m22 = determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
        float m23 = determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

        float m30 = determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
        float m31 = determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
        float m32 = determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
        float m33 = determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
        // clang-format off
        return Matrix4(
            {m00, -m10, m20, -m30}, 
            {-m01, m11, -m21, m31}, 
            {m02, -m12, m22, -m32}, 
            {-m03, m13, -m23, m33});
        // clang-format on
    }

    inline Matrix4 inverse(Matrix4 const m) {
        return adjugate(m) / determinant(m);
    }
} // namespace anton_engine::math

#endif // !CORE_MATH_MARTIX4_HPP_INCLUDE
