#ifndef CORE_MATH_MARTIX4_HPP_INCLUDE
#define CORE_MATH_MARTIX4_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/vector4.hpp>
#include <core/types.hpp>

namespace anton_engine {
    // Row major
    class Matrix4 {
    public:
        static Matrix4 const zero;
        static Matrix4 const identity;

        Matrix4();
        Matrix4(Vector4, Vector4, Vector4, Vector4);

        Vector4& operator[](i32 row);
        Vector4 operator[](i32 row) const;
        f32& operator()(i32 row, i32 column);
        f32 operator()(i32 row, i32 column) const;
        f32 const* get_raw() const;

        Matrix4& operator+=(f32);
        Matrix4& operator-=(f32);
        Matrix4& operator*=(f32);
        Matrix4& operator/=(f32);

        Matrix4& transpose();

    private:
        Vector4 rows[4];
    };

    Matrix4 operator+(Matrix4, f32);
    Matrix4 operator-(Matrix4, f32);
    Matrix4 operator*(Matrix4, f32);
    Matrix4 operator/(Matrix4, f32);
    Matrix4 operator+(Matrix4, Matrix4);
    Matrix4 operator-(Matrix4, Matrix4);
    Matrix4 operator*(Matrix4, Matrix4);
    Vector4 operator*(Vector4, Matrix4);

    namespace math {
        Matrix4 transpose(Matrix4);
        f32 determinant(Matrix4);
        Matrix4 adjugate(Matrix4);
        Matrix4 inverse(Matrix4);
    } // namespace math
} // namespace anton_engine

namespace anton_engine {
    inline Matrix4 const Matrix4::zero = Matrix4();
    inline Matrix4 const Matrix4::identity = Matrix4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

    inline Matrix4::Matrix4(): rows{} {}

    inline Matrix4::Matrix4(Vector4 const a, Vector4 const b, Vector4 const c, Vector4 const d): rows{a, b, c, d} {}

    inline Vector4& Matrix4::operator[](i32 const row) {
        return rows[row];
    }

    inline Vector4 Matrix4::operator[](i32 const row) const {
        return rows[row];
    }

    inline f32& Matrix4::operator()(i32 const row, i32 const column) {
        return rows[row][column];
    }

    inline f32 Matrix4::operator()(i32 const row, i32 const column) const {
        return rows[row][column];
    }

    inline f32 const* Matrix4::get_raw() const {
        return (f32 const*)rows;
    }

    inline Matrix4& Matrix4::operator+=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator-=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator*=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] *= a;
        }
        return *this;
    }

    inline Matrix4& Matrix4::operator/=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
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

    inline Matrix4 operator+(Matrix4 m, f32 const a) {
        m += a;
        return m;
    }

    inline Matrix4 operator-(Matrix4 m, f32 const a) {
        m -= a;
        return m;
    }

    inline Matrix4 operator*(Matrix4 m, f32 const a) {
        m *= a;
        return m;
    }

    inline Matrix4 operator/(Matrix4 m, f32 const a) {
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
        for (i32 i = 0; i < 4; ++i) {
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

    namespace math {
        inline Matrix4 transpose(Matrix4 mat) {
            mat.transpose();
            return mat;
        }

        inline static f32 determinant3x3(f32 m00, f32 m01, f32 m02, f32 m10, f32 m11, f32 m12, f32 m20, f32 m21, f32 m22) {
            return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21;
        }

        inline f32 determinant(Matrix4 const m) {
            f32 det0 = determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 det1 = determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 det2 = determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 det3 = determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
            return m[0][0] * det0 - m[0][1] * det1 + m[0][2] * det2 - m[0][3] * det3;
        }

        inline Matrix4 adjugate(Matrix4 const m) {
            f32 m00 = determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 m01 = determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 m02 = determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 m03 = determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

            f32 m10 = determinant3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 m11 = determinant3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 m12 = determinant3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 m13 = determinant3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

            f32 m20 = determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
            f32 m21 = determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
            f32 m22 = determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
            f32 m23 = determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

            f32 m30 = determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
            f32 m31 = determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
            f32 m32 = determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
            f32 m33 = determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
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
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_MARTIX4_HPP_INCLUDE
