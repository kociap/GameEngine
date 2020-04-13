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

        Matrix2(): rows{} {}
        Matrix2(Vector2 const a, Vector2 const b): rows{a, b} {}

        Vector2& operator[](i32 const row) {
            return rows[row];
        }

        Vector2 operator[](i32 const row) const {
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

        Matrix2& operator+=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] += a;
            }
            return *this;
        }

        Matrix2& operator-=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] -= a;
            }
            return *this;
        }

        Matrix2& operator*=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] *= a;
            }
            return *this;
        }

        Matrix2& operator/=(f32 const a) {
            for (i32 i = 0; i < 4; ++i) {
                rows[i] /= a;
            }
            return *this;
        }

    private:
        Vector2 rows[2];
    };

    inline Matrix2 const zero = Matrix2();
    inline Matrix2 const Matrix2::identity = Matrix2({1, 0}, {0, 1});

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
            atl::swap(m[1][0], m[0][1]);
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

    inline void swap(Matrix2& m1, Matrix2& m2) {
        atl::swap(m1[0], m2[0]);
        atl::swap(m1[1], m2[1]);
    }
} // namespace anton_engine

#endif // !CORE_MATH_MARTIX2_HPP_INCLUDE
