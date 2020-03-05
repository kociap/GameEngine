#ifndef GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE

#include <core/stl/utility.hpp>
#include <core/math/vector2.hpp>

namespace anton_engine {
    // Row major
    class Matrix2 {
    public:
        static Matrix2 const zero;
        static Matrix2 const identity;

        Matrix2();
        Matrix2(Vector2, Vector2);
        Matrix2(float m00, float m01, float m10, float m11);

        Vector2& operator[](int row);
        Vector2 operator[](int row) const;
        float& operator()(int row, int column);
        float operator()(int row, int column) const;
        float const* get_raw() const;

        Matrix2& operator+=(float);
        Matrix2& operator-=(float);
        Matrix2& operator*=(float);
        Matrix2& operator/=(float);

        Matrix2& transpose();

    private:
        Vector2 rows[2];
    };

    Matrix2 operator+(Matrix2, float);
    Matrix2 operator-(Matrix2, float);
    Matrix2 operator*(Matrix2, float);
    Matrix2 operator/(Matrix2, float);
    Matrix2 operator+(Matrix2, Matrix2);
    Matrix2 operator-(Matrix2, Matrix2);
    Matrix2 operator*(Matrix2, Matrix2);
    Vector2 operator*(Vector2, Matrix2);
} // namespace anton_engine

namespace anton_engine::math {
    Matrix2 transpose(Matrix2);
    float determinant(Matrix2);
    Matrix2 adjugate(Matrix2);
    Matrix2 inverse(Matrix2);
} // namespace anton_engine::math

namespace anton_engine {
    inline Matrix2 const zero = Matrix2();
    inline Matrix2 const Matrix2::identity = Matrix2({1, 0}, {0, 1});

    inline Matrix2::Matrix2(): rows{} {}
    inline Matrix2::Matrix2(Vector2 a, Vector2 b): rows{a, b} {}
    inline Matrix2::Matrix2(float m00, float m01, float m10, float m11): rows{{m00, m01}, {m10, m11}} {}

    inline Vector2& Matrix2::operator[](int const row) {
        return rows[row];
    }

    inline Vector2 Matrix2::operator[](int const row) const {
        return rows[row];
    }

    inline float& Matrix2::operator()(int const row, int const column) {
        return rows[row][column];
    }

    inline float Matrix2::operator()(int const row, int const column) const {
        return rows[row][column];
    }

    inline float const* Matrix2::get_raw() const {
        return reinterpret_cast<float const*>(rows);
    }

    inline Matrix2& Matrix2::operator+=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator-=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator*=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] *= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::operator/=(float const a) {
        for (int i = 0; i < 4; ++i) {
            rows[i] /= a;
        }
        return *this;
    }

    inline Matrix2& Matrix2::transpose() {
        anton_stl::swap((*this)[1][0], (*this)[0][1]);
        return *this;
    }

    inline Matrix2 operator+(Matrix2 m, float const a) {
        m += a;
        return m;
    }

    inline Matrix2 operator-(Matrix2 m, float const a) {
        m -= a;
        return m;
    }

    inline Matrix2 operator*(Matrix2 m, float const a) {
        m *= a;
        return m;
    }

    inline Matrix2 operator/(Matrix2 m, float const a) {
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
} // namespace anton_engine

namespace anton_engine::math {
    inline Matrix2 transpose(Matrix2 m) {
        m.transpose();
        return m;
    }

    inline float determinant(Matrix2 const m) {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    }

    inline Matrix2 adjugate(Matrix2 const m) {
        return {{m[1][1], -m[0][1]}, {-m[1][0], m[0][0]}};
    }

    inline Matrix2 inverse(Matrix2 const m) {
        return adjugate(m) / determinant(m);
    }
} // namespace anton_engine::math

#endif // !GAME_ENGINE_CORE_MATH_MARTIX2_HPP_INCLUDE
