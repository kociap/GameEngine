#ifndef GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE

class Quaternion {
public:
    // Identity quaternion.
    // Equivalent to Quaternion(0, 0, 0, 1)
    static const Quaternion identity;

    Quaternion(float, float, float, float);

    // Make this quaternion conjugate
    void conjugate();
    // Normalize this quaternion
    void normalize();
    // Make this quaternion an inverse
    void invert();

    friend Quaternion conjugate(Quaternion const&);
    friend Quaternion inverse(Quaternion const&);
    friend float norm(Quaternion const&);
    friend Quaternion normalized(Quaternion const&);
    friend float norm_squared(Quaternion const&);

    Quaternion& operator+=(Quaternion const&);
    Quaternion& operator-=(Quaternion const&);
    Quaternion& operator*=(Quaternion const&);
    Quaternion& operator*=(float);
    Quaternion& operator/=(float);

    friend Quaternion operator+(Quaternion const&, Quaternion const&);
    friend Quaternion operator-(Quaternion const&, Quaternion const&);
    friend Quaternion operator*(Quaternion const&, Quaternion const&);
    friend Quaternion operator*(Quaternion const&, float);
    friend Quaternion operator/(Quaternion const&, float);

private:
    float x;
    float y;
    float z;
    float w;
};

#endif // !GAME_ENGINE_CORE_MATH_QUATERNION_HPP_INCLUDE
