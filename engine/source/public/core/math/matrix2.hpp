#ifndef CORE_MATH_MARTIX2_HPP_INCLUDE
#define CORE_MATH_MARTIX2_HPP_INCLUDE

#include <core/atl/utility.hpp>
#include <core/math/vector2.hpp>

namespace anton_engine {
    // Row major
    class Matrix2 {
    public:
        static Matrix2 const zero;
        static Matrix2 const identity;

        Matrix2();
        Matrix2(Vector2, Vector2);
        Matrix2(f32 m00, f32 m01, f32 m10, f32 m11);

        Vector2& operator[](i32 row);
        Vector2 operator[](i32 row) const;
        f32& operator()(i32 row, i32 column);
        f32 operator()(i32 row, i32 column) const;
        f32 const* get_raw() const;

        Matrix2& operator+=(f32);
        Matrix2& operator-=(f32);
        Matrix2& operator*=(f32);
        Matrix2& operator/=(f32);

        Matrix2& transpose();

    private:
        Vector2 rows[2];
    };

    Matrix2 operator+(Matrix2, f32);
    Matrix2 operator-(Matrix2, f32);
    Matrix2 operator*(Matrix2, f32);
    Matrix2 operator/(Matrix2, f32);
    Matrix2 operator+(Matrix2, Matrix2);
    Matrix2 operator-(Matrix2, Matrix2);
    Matrix2 operator*(Matrix2, Matrix2);
    Vector2 operator*(Vector2, Matrix2);

    namespace math {
        Matrix2 transpose(Matrix2);
        f32 determinant(Matrix2);
        Matrix2 adjugate(Matrix2);
        Matrix2 inverse(Matrix2);
    } // namespace math
} // namespace anton_engine

namespace anton_engine {
    inline Matrix2 const zero = Matrix2();
    inline Matrix2 const Matrix2::identity = Matrix2({1, 0}, {0, 1});

    inline Matrix2::Matrix2(): rows{} {}
    inline Matrix2::Matrix2(Vector2 a, Vector2 b): rows{a, b} {}
    inline Matrix2::Matrix2(f32 m00, f32 m01, f32 m10, f32 m11): rows{{m00, m01}, {m10, m11}} {}

    inline Vector2& Matrix2::operator[](i32 const row) {
        return rows[row];
    }

    inline Vector2 Matrix2::operator[](i32 const row) const {
        return rows[row];
    }

    inline f32& Matrix2::operator()(i32 const row, i32 const column) {
        return rows[row][column];
    }

    inline f32 Matrix2::operator()(i32 const row, i32 const column) const {
        return rows[row][column];
    }

    inline f32 const* Matrix2::get_raw() const {
        return (f32 const*)rows;
    }

    inline Matrix2& Matrix2::operator+=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator-=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator*=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] *= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator/=(f32 const a) {
        for (i32 i = 0; i < 4; ++i) {
            rows[i] /= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::transpose() {
        atl::swap((*this)[1][0], (*this)[0][1]);
        return *this;
    }

    inline Matrix2 operator+(Matrix2 m, f32 const a) {
        m += a;
        return m;
    }

    inline Matrix2 operator-(Matrix2 m, f32 const a) {
        m -= a;
        return m;
    }

    inline Matrix2 operator*(Matrix2 m, f32 const a) {
        m *= a;
        return m;
    }

    inline Matrix2 operator/(Matrix2 m, f32 const a) {
        m /= a;
        return m;
    }

    inline Matrix2 operator+(Matrix2 m1, Matrix2 const m2) {
        m1[0][0] += m2[0][0];
        m1[0][1] += m2[0][1];
        m1[1][0] += m2[1][0];
        m1[1][1] += m2[1][1];
        return m1;
    }

    inline Matrix2 operator-(Matrix2 m1, Matrix2 const m2) {
        m1[0][0] -= m2[0][0];
        m1[0][1] -= m2[0][1];
        m1[1][0] -= m2[1][0];
        m1[1][1] -= m2[1][1];
        return m1;
    }

    inline Matrix2 operator*(Matrix2 const m1, Matrix2 const m2) {
        return {{m1[0][0] * m2[0][0] + m1[0][1] * m2[1][0], m1[0][0] * m2[0][1] + m1[0][1] * m2[1][1]},
                {m1[1][0] * m2[0][0] + m1[1][1] * m2[1][0], m1[1][0] * m2[0][1] + m1[1][1] * m2[1][1]}};
    }

    inline Vector2 operator*(Vector2 const v, Matrix2 const m) {
        return {v[0] * m[0][0] + v[1] * m[1][0], v[0] * m[0][1] + v[1] * m[1][1]};
    }

    namespace math {
        inline Matrix2 transpose(Matrix2 m) {
            m.transpose();
            return m;
        }

        inline f32 determinant(Matrix2 const m) {
            return m[0][0] * m[1][1] - m[1][0] * m[0][1];
        }

        inline Matrix2 adjugate(Matrix2 const m) {
            return {{m[1][1], -m[0][1]}, {-m[1][0], m[0][0]}};
        }

        inline Matrix2 inverse(Matrix2 const m) {
            return adjugate(m) / determinant(m);
        }
    } // namespace math
} // namespace anton_engine

#endif // !CORE_MATH_MARTIX2_HPP_INCLUDE
