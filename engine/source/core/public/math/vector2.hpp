#ifndef CORE_MATH_VECTOR2_HPP_INCLUDE
#define CORE_MATH_VECTOR2_HPP_INCLUDE

namespace anton_engine {
    class Vector3;
    class Vector4;

    class Vector2 {
    public:
        static ENGINE_API Vector2 const zero;
        static ENGINE_API Vector2 const one;
        static ENGINE_API Vector2 const up;
        static ENGINE_API Vector2 const right;

        float x = 0;
        float y = 0;

        Vector2() = default;
        Vector2(float x, float y);
        explicit Vector2(Vector3 const&);
        explicit Vector2(Vector4 const&);

        float& component(int);
        float component(int) const;
        float& operator[](int);
        float operator[](int) const;

        Vector2& operator-();
        Vector2 operator-() const;
        Vector2& operator+=(Vector2);
        Vector2& operator-=(Vector2);
        Vector2& operator+=(float);
        Vector2& operator-=(float);
        Vector2& operator*=(float);
        Vector2& operator/=(float);

        // Check if all components are equal 0
        bool is_zero() const;
        bool is_almost_zero(float tolerance = 0.00001f) const;

        float length_squared() const;
        float length() const;

        // If vector is non-zero, normalizes the vector.
        // Otherwise leaves it unchanged
        Vector2& normalize();

        Vector2& multiply_componentwise(Vector2 const&);
    };

    Vector2 operator+(Vector2, Vector2);
    Vector2 operator-(Vector2, Vector2);
    Vector2 operator+(Vector2, float);
    Vector2 operator-(Vector2, float);
    Vector2 operator*(Vector2, float);
    Vector2 operator*(float, Vector2);
    Vector2 operator/(Vector2, float);
    bool operator==(Vector2, Vector2);
    bool operator!=(Vector2, Vector2);

    void swap(Vector2&, Vector2&);
} // namespace anton_engine

namespace anton_engine::math {
    float dot(Vector2 const& vec1, Vector2 const& vec2);

    // If vector is non-zero, returns normalized copy of the vector.
    // Otherwise returns zero vector
    Vector2 normalize(Vector2);
    float length(Vector2);

    Vector2 multiply_componentwise(Vector2 const&, Vector2 const&);
} // namespace anton_engine::math

#endif // !CORE_MATH_VECTOR2_HPP_INCLUDE
