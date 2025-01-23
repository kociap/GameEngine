#pragma once

#include <anton/type_traits.hpp>

namespace anton_engine::utils {
  template<typename T>
  constexpr anton::underlying_type<T> enum_to_value(T v)
  {
    return static_cast<anton::underlying_type<T>>(v);
  }

  template<typename T>
  struct enable_enum_add_operator {};

  template<typename T>
  struct enable_enum_bitwise_and {};

  template<typename T>
  struct enable_enum_bitwise_or {};
} // namespace anton_engine::utils

template<typename T,
         bool = anton_engine::utils::enable_enum_add_operator<T>::value>
constexpr T operator+(T a, anton::underlying_type<T> b)
{
  return static_cast<T>(anton_engine::utils::enum_to_value(a) + b);
}

template<typename T,
         bool = anton_engine::utils::enable_enum_add_operator<T>::value>
constexpr T operator+(anton::underlying_type<T> a, T b)
{
  return static_cast<T>(a + anton_engine::utils::enum_to_value(b));
}

template<typename T,
         bool = anton_engine::utils::enable_enum_bitwise_and<T>::value>
constexpr T operator&(T a, T b)
{
  return static_cast<T>(anton_engine::utils::enum_to_value(a) &
                        anton_engine::utils::enum_to_value(b));
}

template<typename T,
         bool = anton_engine::utils::enable_enum_bitwise_or<T>::value>
constexpr T operator|(T const a, T const b)
{
  return static_cast<T>(anton_engine::utils::enum_to_value(a) |
                        anton_engine::utils::enum_to_value(b));
}
