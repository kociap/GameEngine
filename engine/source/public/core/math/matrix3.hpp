#ifndef CORE_MATH_MARTIX3_HPP_INCLUDE
#define CORE_MATH_MARTIX3_HPP_INCLUDE

#include <core/math/vector3.hpp>

namespace anton_engine {
    // Row major
    class Matrix3 {
    public:
        static Matrix3 const zero;
        static Matrix3 const identity;

        Matrix3();
        Matrix3(Vector3, Vector3, Vector3);

        Vector3& operator[](i32 row);
        Vector3 operator[](i32 row) const;
        f32& operator()(i32 row, i32 column);
        f32 operator()(i32 row, i32 column) const;
        f32 const* get_raw() const;

        Matrix3& operator+=(f32);
        Matrix3& operator-=(f32);
        Matrix3& operator*=(f32);
        Matrix3& operator/=(f32);

        Matrix3& transpose();

    private:
        Vector3 rows[3];
    };

    Matrix3 operator+(Matrix3, f32);
    Matrix3 operator-(Matrix3, f32);
    Matrix3 operator*(Matrix3, f32);
    Matrix3 operator/(Matrix3, f32);
    Matrix3 operator+(Matrix3, Matrix3);
    Matrix3 operator-(Matrix3, Matrix3);
    Matrix3 operator*(Matrix3, Matrix3);
    Vector3 operator*(Vector3, Matrix3);

    namespace math {
        Matrix3 transpose(Matrix3);
        f32 determinant(Matrix3);
        Matrix3 adjugate(Matrix3);
        Matrix3 inverse(Matrix3);
    } // namespace math
} // namespace anton_engine

namespace anton_engine {
    inline Matrix3 const Matrix3::zero = Matrix3();
    inline Matrix3 const Matrix3::identity = Matrix3({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

    Matrix3::Matrix3(): rows{} {}
    Matrix3::Matrix3(Vector3 const a, Vector3 const b, Vector3 const c): rows{a, b, c} {}

    Vector3& Matrix3::operator[](i32 const row) {
        return rows[row];
    }

    Vector3 Matrix3::operator[](i32 const row) const {
        return rows[row];
    }

    inline f32& Matrix3::operator()(i32 const row, i32 const column) {
        return rows[row][column];
    }

    inline f32 Matrix3::operator()(i32 const row, i32 const column) const {
        return rows[row][column];
    }

    inline f32 const* Matrix3::get_raw() const {
        return (f32 const*)rows;
    }

    inline Matrix3& Matrix3::operator+=(f32 a) {
        for (i32 i = 0; i < 3; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator-=(f32 a) {
        for (i32 i = 0; i < 3; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator*=(f32 num) {
        for (i32 i = 0; i < 3; ++i) {
            rows[i] *= num;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator/=(f32 num) {
        for (i32 i = 0; i < 3; ++i) {
            rows[i] /= num;
        }
        return *this;
    }

    inline Matrix3& Matrix3::transpose() {
        atl::swap((*this)[0][1], (*this)[1][0]);
        atl::swap((*this)[0][2], (*this)[2][0]);
        atl::swap((*this)[1][2], (*this)[2][1]);
        return *this;
    }

    inline Matrix3 operator+(Matrix3 m, f32 a) {
        m += a;
        return m;
    }

    inline Matrix3 operator-(Matrix3 m, f32 a) {
        m -= a;
        return m;
    }

    inline Matrix3 operator*(Matrix3 m, f32 a) {
        m *= a;
        return m;
    }

    inline Matrix3 operator/(Matrix3 m, f32 a) {
        m /= a;
        return m;
    }

    inline Matrix3 operator+(Matrix3 a, Matrix3 b) {
        return {{a[0][0] + b[0][0], a[0][1] + b[0][1], a[0][2] + b[0][2]},
                {a[1][0] + b[1][0], a[1][1] + b[1][1], a[1][2] + b[1][2]},
                {a[2][0] + b[2][0], a[2][1] + b[2][1], a[2][2] + b[2][2]}};
    }

    inline Matrix3 operator-(Matrix3 a, Matrix3 b) {
        return {{a[0][0] - b[0][0], a[0][1] - b[0][1], a[0][2] - b[0][2]},
                {a[1][0] - b[1][0], a[1][1] - b[1][1], a[1][2] - b[1][2]},
                {a[2][0] - b[2][0], a[2][1] - b[2][1], a[2][2] - b[2][2]}};
    }

    inline Matrix3 operator*(Matrix3 const lhs, Matrix3 const rhs) {
        Matrix3 r;
        for (i32 i = 0; i < 3; ++i) {
            r[i][0] = lhs[i][0] * rhs[0][0] + lhs[i][1] * rhs[1][0] + lhs[i][2] * rhs[2][0];
            r[i][1] = lhs[i][0] * rhs[0][1] + lhs[i][1] * rhs[1][1] + lhs[i][2] * rhs[2][1];
            r[i][2] = lhs[i][0] * rhs[0][2] + lhs[i][1] * rhs[1][2] + lhs[i][2] * rhs[2][2];
        }
        return r;
    }

    inline Vector3 operator*(Vector3 const lhs, Matrix3 const rhs) {
        Vector3 r;
        r[0] = lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0] + lhs[2] * rhs[2][0];
        r[1] = lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] + lhs[2] * rhs[2][1];
        r[2] = lhs[0] * rhs[0][2] + lhs[1] * rhs[1][2] + lhs[2] * rhs[2][2];
        return r;
    }

    namespace math {
        inline Matrix3 transpose(Matrix3 m) {
            m.transpose();
            return m;
        }

        inline static f32 determinant2x2(f32 m00, f32 m01, f32 m10, f32 m11) {
            return m00 * m11 - m01 * m10;
        }

        inline f32 determinant(Matrix3 m) {
            // clang-format off
            return   m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
                - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
            // clang-format on
        }

        inline Matrix3 adjugate(Matrix3 m) {
            f32 m00 = determinant2x2(m[1][1], m[1][2], m[2][1], m[2][2]);
            f32 m01 = determinant2x2(m[1][0], m[1][2], m[2][0], m[2][2]);
            f32 m02 = determinant2x2(m[1][0], m[1][1], m[2][0], m[2][1]);

            f32 m10 = determinant2x2(m[0][1], m[0][2], m[2][1], m[2][2]);
            f32 m11 = determinant2x2(m[0][0], m[0][2], m[2][0], m[2][2]);
            f32 m12 = determinant2x2(m[0][0], m[0][1], m[2][0], m[2][1]);

            f32 m20 = determinant2x2(m[0][1], m[0][2], m[1][1], m[1][2]);
            f32 m21 = determinant2x2(m[0][0], m[0][2], m[1][0], m[1][2]);
            f32 m22 = determinant2x2(m[0][0], m[0][1], m[1][0], m[1][1]);
            // clang-format off
            return Matrix3(
                {m00, -m10, m20}, 
                {-m01, m11, -m21}, 
                {m02, -m12, m22});
            // clang-format on
        }

        inline Matrix3 inverse(Matrix3 m) {
            return adjugate(m) / determinant(m);
        }
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_MARTIX3_HPP_INCLUDE
