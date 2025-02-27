#pragma once

#include <anton/swap.hpp>
#include <anton/utility.hpp>
#include <core/types.hpp>

namespace anton_engine {
  class Color {
  public:
    static Color const white;
    static Color const black;
    static Color const red;
    static Color const green;
    static Color const blue;

    f32 r = 0;
    f32 g = 0;
    f32 b = 0;
    f32 a = 1.0f;

    Color() = default;
    Color(f32 red, f32 green, f32 blue, f32 alpha = 1.0f)
      : r(red), g(green), b(blue), a(alpha)
    {
    }

    // Componentwise multiply
    void operator*=(Color const&);
  };

  // Componentwise multiply
  Color operator*(Color const&, Color const&);

  void swap(Color&, Color&);
} // namespace anton_engine

namespace anton_engine {
  inline Color const Color::white = Color(1.0f, 1.0f, 1.0f);
  inline Color const Color::black = Color(0.0f, 0.0f, 0.0f);
  inline Color const Color::red = Color(1.0f, 0.0f, 0.0f);
  inline Color const Color::green = Color(0.0f, 1.0f, 0.0f);
  inline Color const Color::blue = Color(0.0f, 0.0f, 1.0f);

  inline void Color::operator*=(Color const& c)
  {
    r *= c.r;
    g *= c.g;
    b *= c.b;
  }

  inline Color operator*(Color const& c1, Color const& c2)
  {
    Color c(c1);
    c *= c2;
    return c;
  }

  inline void swap(Color& a, Color& b)
  {
    anton::swap(a.r, b.r);
    anton::swap(a.g, b.g);
    anton::swap(a.b, b.b);
    anton::swap(a.a, b.a);
  }
} // namespace anton_engine
