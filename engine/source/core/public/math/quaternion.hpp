#ifndef GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE

class Quaternion {
public:
    // Identity quaternion.
    // Equivalent to Quaternion(0, 0, 0, 1)
    static ENGINE_API Quaternion const identity;

public:
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

public:
    Quaternion() = default;
    Quaternion(float x, float y, float z, float w);

    // Make this quaternion conjugate
    void conjugate();
    // Normalize this quaternion
    void normalize();
    // Make this quaternion an inverse
    void inverse();
};

Quaternion operator+(Quaternion const&, Quaternion const&);
Quaternion operator-(Quaternion const&, Quaternion const&);
// Hamilton Product
Quaternion operator*(Quaternion, Quaternion);
Quaternion operator*(Quaternion const&, float);
Quaternion operator/(Quaternion const&, float);

namespace math {
    Quaternion conjugate(Quaternion const&);
    // If quaternion is normalized, this function returns the same resuls as conjugate
    Quaternion inverse(Quaternion const&);
    Quaternion normalize(Quaternion const&);
    float length(Quaternion const&);
    float length_squared(Quaternion const&);
} // namespace math

#endif // !GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE
