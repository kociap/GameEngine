#ifndef CORE_COLOR_HPP_INCLUDE
#define CORE_COLOR_HPP_INCLUDE

class Color {
public:
    static ENGINE_API Color const white;
    static ENGINE_API Color const black;
    static ENGINE_API Color const red;
    static ENGINE_API Color const green;
    static ENGINE_API Color const blue;

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

void swap(Color&, Color&);

#endif // !CORE_COLOR_HPP_INCLUDE