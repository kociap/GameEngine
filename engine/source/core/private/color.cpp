#include <anton_stl/utility.hpp>
#include <color.hpp>

namespace anton_engine {
    Color const Color::white = Color(1.0f, 1.0f, 1.0f);
    Color const Color::black = Color(0.0f, 0.0f, 0.0f);
    Color const Color::red = Color(1.0f, 0.0f, 0.0f);
    Color const Color::green = Color(0.0f, 1.0f, 0.0f);
    Color const Color::blue = Color(0.0f, 0.0f, 1.0f);

    Color::Color() {}
    Color::Color(float r, float g, float b, float a): r(r), g(g), b(b), a(a) {}

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

    void swap(Color& a, Color& b) {
        anton_stl::swap(a.r, b.r);
        anton_stl::swap(a.g, b.g);
        anton_stl::swap(a.b, b.b);
        anton_stl::swap(a.a, b.a);
    }
} // namespace anton_engine
