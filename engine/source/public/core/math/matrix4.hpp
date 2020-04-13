#ifndef CORE_MATH_MARTIX4_HPP_INCLUDE
#define CORE_MATH_MARTIX4_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/vector4.hpp>

namespace anton_engine {
    // Row major
    class Matrix4 {
    public:
        static Matrix4 const zero;
        static Matrix4 const identity;

        Matrix4(): rows{} {}
        Matrix4(Vector4 const a, Vector4 const b, Vector4 const c, Vector4 const d): rows{a, b, c, d} {}

        Vector4& operator[](i32 const row) {
            return rows[row];
        }

        Vector4 operator[](i32 const row) const {
            return rows[row];
        }

        f32& operator()(i32 const row, i32 const column) {
            return rows[row][column];
        }

        f32 operator()(i32 const row, i32 const column) const {
            return rows[row][column];
        }

        f32 const* get_raw() const {
            return (f32 const*)rows;
        }

        Matrix4& operator+=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] += a;
            }
            return *this;
        }

        Matrix4& operator-=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] -= a;
            }
            return *this;
        }

        Matrix4& operator*=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] *= a;
            }
            return *this;
        }

        Matrix4& operator/=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] /= a;
            }
            return *this;
        }

    private:
        Vector4 rows[4];
    };

    inline Matrix4 const Matrix4::zero = Matrix4();
    inline Matrix4 const Matrix4::identity = Matrix4{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

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
            atl::swap(mat[0][1], mat[1][0]);
            atl::swap(mat[0][2], mat[2][0]);
            atl::swap(mat[0][3], mat[3][0]);
            atl::swap(mat[1][2], mat[2][1]);
            atl::swap(mat[1][3], mat[3][1]);
            atl::swap(mat[2][3], mat[3][2]);
            return mat;
        }

        inline static f32 _determinant3x3(f32 m00, f32 m01, f32 m02, f32 m10, f32 m11, f32 m12, f32 m20, f32 m21, f32 m22) {
            return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21;
        }

        inline f32 determinant(Matrix4 const m) {
            f32 det0 = _determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 det1 = _determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 det2 = _determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 det3 = _determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
            return m[0][0] * det0 - m[0][1] * det1 + m[0][2] * det2 - m[0][3] * det3;
        }

        inline Matrix4 adjugate(Matrix4 const m) {
            f32 m00 = _determinant3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 m01 = _determinant3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 m02 = _determinant3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 m03 = _determinant3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

            f32 m10 = _determinant3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
            f32 m11 = _determinant3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
            f32 m12 = _determinant3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
            f32 m13 = _determinant3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);

            f32 m20 = _determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
            f32 m21 = _determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
            f32 m22 = _determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
            f32 m23 = _determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);

            f32 m30 = _determinant3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);
            f32 m31 = _determinant3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);
            f32 m32 = _determinant3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);
            f32 m33 = _determinant3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
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

    inline void swap(Matrix4& m1, Matrix4& m2) {
        atl::swap(m1[0], m2[0]);
        atl::swap(m1[1], m2[1]);
        atl::swap(m1[2], m2[2]);
        atl::swap(m1[3], m2[3]);
    }
} // namespace anton_engine

#endif // !CORE_MATH_MARTIX4_HPP_INCLUDE
