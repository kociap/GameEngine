#pragma once

#include <anton/types.hpp>

namespace anton::math {
  struct Vec2;
  struct Vec3;
  struct Vec4;
  struct Mat2;
  struct Mat3;
  struct Mat4;
  struct Quat;
} // namespace anton::math

namespace anton_engine {
  using i8 = anton::i8;
  using i16 = anton::i16;
  using i32 = anton::i32;
  using i64 = anton::i64;

  using u8 = anton::u8;
  using u16 = anton::u16;
  using u32 = anton::u32;
  using u64 = anton::u64;

  using f32 = anton::f32;
  using f64 = anton::f64;

  using isize = anton::isize;
  using usize = anton::usize;

  using char8 = anton::char8;
  using char16 = anton::char16;
  using char32 = anton::char32;

  using nullptr_t = anton::nullptr_t;

  using anton::math::Mat2;
  using anton::math::Mat3;
  using anton::math::Mat4;
  using anton::math::Quat;
  using anton::math::Vec2;
  using anton::math::Vec3;
  using anton::math::Vec4;

  namespace math {
    using namespace anton::math;
  }

  template<typename T>
  struct Rect {
    T left;
    T top;
    T right;
    T bottom;
  };
} // namespace anton_engine
