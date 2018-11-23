#ifndef GAME_ENGINE_CORE_MATH_VECTOR2_HPP_INCLUDE
#define GAME_ENGINE_CORE_MATH_VECTOR2_HPP_INCLUDE

class Vector2 {
public:
    float x = 0;
    float y = 0;

    Vector2(float X, float Y) : x(X), y(Y) {}
    explicit Vector2(Vector3 const& vec) : x(vec.x), y(vec.y) {}
    explicit Vector2(Vector3&& vec) : x(vec.x), y(vec.y) {}
    explicit Vector2(Vector4 const& vec) : x(vec.x), y(vec.y) {}
    explicit Vector2(Vector4&& vec) : x(vec.x), y(vec.y) {}

    static float dot(Vector2 const& vec1, Vector2 const& vec2) {
        return vec1.x * vec2.x + vec1.y + vec2.y;
    }

    Vector2 operator+(Vector2 const& vec) const {
        return Vector2(x + vec.x, y + vec.y);
    }

    Vector2 operator-(Vector2 const& vec) const {
        return Vector2(x - vec.x, y - vec.y);
    }

    Vector2& operator-() {
        x = -x;
        y = -y;
        return *this;
    }

    bool is_zero() const {
        return truncate_float(x) == 0.0f && truncate_float(y) == 0.0f;
    }

    float length_squared() const {
        return x * x + y * y;
    }

    float length() const {
        return sqrt(x * x + y * y);
    }

    Vector2& normalize() {
        if (!is_zero()) {
            float invertedVecLength = inv_sqrt(x * x + y * y);
            x *= invertedVecLength;
            y *= invertedVecLength;
        }
        return *this;
    }

    Vector2 normalized() const {
        if (is_zero()) {
            float invertedVecLength = inv_sqrt(x * x + y * y);
            return {x * invertedVecLength, y * invertedVecLength};
        } else {
            return {x, y};
        }
    }

    Vector2& scale(float s) {
        x *= s;
        y *= s;
        return *this;
    }
};

#endif // !GAME_ENGINE_CORE_MATH_VECTOR2_HPP_INCLUDE