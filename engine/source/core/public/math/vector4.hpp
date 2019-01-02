#ifndef GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE

class Vector4 {
public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 0;

    Vector4();
    Vector4(float x, float y, float z, float w);

    float& component(int);
    float const& component(int) const;

    Vector4& operator*=(float);

    bool is_zero() const;

    float length_squared() const;
    float length() const;

    // If vector is non-zero, normalizes the vector.
    // Otherwise leaves it unchanged
    Vector4& normalize();
};

// If vector is non-zero, returns normalized copy of the vector.
// Otherwise returns zero vector
Vector4 normalize(Vector4);

void swap(Vector4&, Vector4&);

#endif // !GAME_ENGINE_CORE_MATH_VECTOR4_HPP_INCLUDE
