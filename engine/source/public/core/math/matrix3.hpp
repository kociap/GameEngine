#ifndef GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE

#include <core/math/vector3.hpp>

namespace anton_engine {
    // Row major
    class Matrix3 {
    public:
        static Matrix3 const zero;
        static Matrix3 const identity;

        Matrix3();
        Matrix3(Vector3, Vector3, Vector3);

        Vector3& operator[](int row);
        Vector3 operator[](int row) const;
        float& operator()(int row, int column);
        float operator()(int row, int column) const;
        float const* get_raw() const;

        Matrix3& operator+=(float);
        Matrix3& operator-=(float);
        Matrix3& operator*=(float);
        Matrix3& operator/=(float);

        Matrix3& transpose();

    private:
        Vector3 rows[3];
    };

    Matrix3 operator+(Matrix3, float);
    Matrix3 operator-(Matrix3, float);
    Matrix3 operator*(Matrix3, float);
    Matrix3 operator/(Matrix3, float);
    Matrix3 operator+(Matrix3, Matrix3);
    Matrix3 operator-(Matrix3, Matrix3);
    Matrix3 operator*(Matrix3, Matrix3);
    Vector3 operator*(Vector3, Matrix3);
} // namespace anton_engine

namespace anton_engine::math {
    Matrix3 transpose(Matrix3);
    float determinant(Matrix3);
    Matrix3 adjugate(Matrix3);
    Matrix3 inverse(Matrix3);
} // namespace anton_engine::math

namespace anton_engine {
    inline Matrix3 const Matrix3::zero = Matrix3();
    inline Matrix3 const Matrix3::identity = Matrix3({1, 0, 0}, {0, 1, 0}, {0, 0, 1});

    Matrix3::Matrix3(): rows{} {}
    Matrix3::Matrix3(Vector3 const a, Vector3 const b, Vector3 const c): rows{a, b, c} {}

    Vector3& Matrix3::operator[](int const row) {
        return rows[row];
    }

    Vector3 Matrix3::operator[](int const row) const {
        return rows[row];
    }

    inline float& Matrix3::operator()(int const row, int const column) {
        return rows[row][column];
    }

    inline float Matrix3::operator()(int const row, int const column) const {
        return rows[row][column];
    }

    inline float const* Matrix3::get_raw() const {
        return reinterpret_cast<float const*>(rows);
    }

    inline Matrix3& Matrix3::operator+=(float a) {
        for (int i = 0; i < 3; ++i) {
            rows[i] += a;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator-=(float a) {
        for (int i = 0; i < 3; ++i) {
            rows[i] -= a;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator*=(float num) {
        for (int i = 0; i < 3; ++i) {
            rows[i] *= num;
        }
        return *this;
    }

    inline Matrix3& Matrix3::operator/=(float num) {
        for (int i = 0; i < 3; ++i) {
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

    inline Matrix3 operator+(Matrix3 m, float a) {
        m += a;
        return m;
    }

    inline Matrix3 operator-(Matrix3 m, float a) {
        m -= a;
        return m;
    }

    inline Matrix3 operator*(Matrix3 m, float a) {
        m *= a;
        return m;
    }

    inline Matrix3 operator/(Matrix3 m, float a) {
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
        for (int i = 0; i < 3; ++i) {
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
} // namespace anton_engine

namespace anton_engine::math {
    inline Matrix3 transpose(Matrix3 m) {
        m.transpose();
        return m;
    }

    inline static float determinant2x2(float m00, float m01, float m10, float m11) {
        return m00 * m11 - m01 * m10;
    }

    inline float determinant(Matrix3 m) {
        // clang-format off
        return   m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
               - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
               + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
        // clang-format on
    }

    inline Matrix3 adjugate(Matrix3 m) {
        float m00 = determinant2x2(m[1][1], m[1][2], m[2][1], m[2][2]);
        float m01 = determinant2x2(m[1][0], m[1][2], m[2][0], m[2][2]);
        float m02 = determinant2x2(m[1][0], m[1][1], m[2][0], m[2][1]);

        float m10 = determinant2x2(m[0][1], m[0][2], m[2][1], m[2][2]);
        float m11 = determinant2x2(m[0][0], m[0][2], m[2][0], m[2][2]);
        float m12 = determinant2x2(m[0][0], m[0][1], m[2][0], m[2][1]);

        float m20 = determinant2x2(m[0][1], m[0][2], m[1][1], m[1][2]);
        float m21 = determinant2x2(m[0][0], m[0][2], m[1][0], m[1][2]);
        float m22 = determinant2x2(m[0][0], m[0][1], m[1][0], m[1][1]);
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
} // namespace anton_engine::math

#endif // !GAME_ENGINE_CORE_MATH_MARTIX3_HPP_INCLUDE
