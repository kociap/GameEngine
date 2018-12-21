#ifndef CORE_COLOR_HPP_INCLUDE
#define CORE_COLOR_HPP_INCLUDE

class Color {
public:
    float r = 0;
    float g = 0;
    float b = 0;

	Color();
	Color(float red, float green, float blue);

    Color& operator*=(Color const&);
};

Color operator*(Color const&, Color const&);

#endif // !CORE_COLOR_HPP_INCLUDE