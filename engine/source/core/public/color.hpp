#ifndef CORE_COLOR_HPP_INCLUDE
#define CORE_COLOR_HPP_INCLUDE

class Color {
public:
    static Color const white;
    static Color const black;
    static Color const red;
    static Color const green;
    static Color const blue;

    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.0f;

    Color();
    Color(float red, float green, float blue, float alpha = 1.0f);

    // Componentwise multiply
    Color& operator*=(Color const&);
};

// Componentwise multiply
Color operator*(Color const&, Color const&);

#endif // !CORE_COLOR_HPP_INCLUDE