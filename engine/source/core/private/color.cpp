#include "color.hpp"

Color const Color::white = Color(1.0f, 1.0f, 1.0f);
Color const Color::black = Color(0.0f, 0.0f, 0.0f);
Color const Color::red = Color(1.0f, 0.0f, 0.0f);
Color const Color::green = Color(0.0f, 1.0f, 0.0f);
Color const Color::blue = Color(0.0f, 0.0f, 1.0f);

Color::Color() {}
Color::Color(float red, float green, float blue) : r(red), g(green), b(blue) {}

Color& Color::operator*=(Color const& c) {
    r *= c.r;
    g *= c.g;
    b *= c.b;
    return *this;
}

Color operator*(Color const& c1, Color const& c2) {
    Color c(c1);
    c *= c2;
    return c;
}
