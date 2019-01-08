#ifndef CORE_MATH_VECTOR2_HPP_INCLUDE
#define CORE_MATH_VECTOR2_HPP_INCLUDE

class Vector2 {
public:
    static const Vector2 zero;
    static const Vector2 one;
    static const Vector2 up;
    static const Vector2 right;

    static float dot(Vector2 const& vec1, Vector2 const& vec2);

    float x = 0;
    float y = 0;

    Vector2() = default;
    Vector2(float x, float y);

    Vector2& operator-();
    Vector2 operator-() const;
    Vector2& operator+=(Vector2 const&);
    Vector2& operator-=(Vector2 const&);
    Vector2& operator*=(float);

    // Check if all components are equal 0
    bool is_zero() const;

    float length_squared() const;
    float length() const;

    // If vector is non-zero, normalizes the vector.
    // Otherwise leaves it unchanged
    Vector2& normalize();

    Vector2& scale(float s);
    Vector2& multiply_componentwise(Vector2 const&);
};

Vector2 operator+(Vector2 const&, Vector2 const&);
Vector2 operator-(Vector2 const&, Vector2 const&);
Vector2 operator*(Vector2 const&, float);
void swap(Vector2&, Vector2&);

// If vector is non-zero, returns normalized copy of the vector.
// Otherwise returns zero vector
Vector2 normalize(Vector2);

Vector2 multiply_componentwise(Vector2 const&, Vector2 const&);

#endif // !CORE_MATH_VECTOR2_HPP_INCLUDE