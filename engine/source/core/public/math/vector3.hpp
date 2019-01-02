#ifndef CORE_MATH_VECTOR3_HPP_INCLUDE
#define CORE_MATH_VECTOR3_HPP_INCLUDE

class Vector3 {
public:
    static Vector3 const zero;
    static Vector3 const one;
    static Vector3 const up;
    static Vector3 const right;
    static Vector3 const forward;

    static float dot(Vector3 const& vec1, Vector3 const& vec2);
    static Vector3 cross(Vector3 const& vec1, Vector3 const& vec2);
    static Vector3 cross_direction(Vector3 const& vec1, Vector3 const& vec2) {}

    float x = 0;
    float y = 0;
    float z = 0;

    Vector3();
    Vector3(float x, float y, float z);

    float& component(int);
    float const& component(int) const;

    Vector3& operator-();
    Vector3& operator+=(Vector3 const&);
    Vector3& operator-=(Vector3 const&);
    Vector3& operator*=(float);

    bool is_zero() const;
    float length_squared() const;
    float length() const;

    // If vector is non-zero, normalizes the vector.
    // Otherwise leaves it unchanged
    Vector3& normalize();
    Vector3& scale(float);
};

Vector3 operator+(Vector3 const&, Vector3 const&);
Vector3 operator-(Vector3 const&, Vector3 const&);
Vector3 operator*(Vector3 const&, float);

// If vector is non-zero, returns normalized copy of the vector.
// Otherwise returns zero vector
Vector3 normalize(Vector3);

void swap(Vector3&, Vector3&);

#endif // !CORE_MATH_VECTOR3_HPP_INCLUDE