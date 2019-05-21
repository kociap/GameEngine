#ifndef CORE_MATH_VECTOR4_HPP_INCLUDE
#define CORE_MATH_VECTOR4_HPP_INCLUDE

class Vector2;
class Vector3;

class Vector4 {
public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    Vector4() = default;
    Vector4(float x, float y, float z, float w);
    explicit Vector4(Vector2 const&, float z = 0, float w = 0);
    explicit Vector4(Vector3 const&, float w = 0);

    float& component(int);
    float component(int) const;

    Vector4& operator-();
    Vector4 operator-() const;
    Vector4& operator+=(Vector4);
    Vector4& operator-=(Vector4);
    Vector4& operator+=(float);
    Vector4& operator-=(float);
    Vector4& operator*=(float);
    Vector4& operator/=(float);

    // Check if all components are equal 0
    bool is_zero() const;
    bool is_almost_zero(float tolerance = 0.00001f) const;

    float length_squared() const;
    float length() const;

    // If vector is non-zero, normalizes the vector.
    // Otherwise leaves it unchanged
    Vector4& normalize();

    Vector4& multiply_componentwise(Vector4 const&);
};

Vector4 operator+(Vector4, Vector4);
Vector4 operator-(Vector4, Vector4);
Vector4 operator+(Vector4, float);
Vector4 operator-(Vector4, float);
Vector4 operator*(Vector4, float);
Vector4 operator*(float, Vector4);
Vector4 operator/(Vector4, float);
bool operator==(Vector4, Vector4);
bool operator!=(Vector4, Vector4);

void swap(Vector4&, Vector4&);

namespace math {
    float dot(Vector4, Vector4);

    // If vector is non-zero, returns normalized copy of the vector.
    // Otherwise returns zero vector
    Vector4 normalize(Vector4);
    float length(Vector4);

    Vector4 multiply_componentwise(Vector4 const&, Vector4 const&);
} // namespace math

#endif // !CORE_MATH_VECTOR4_HPP_INCLUDE
