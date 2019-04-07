#ifndef CORE_UTILS_ENUM_HPP_INCLUDE
#define CORE_UTILS_ENUM_HPP_INCLUDE

#include <type_traits>

namespace utils {
    template <typename T>
    constexpr std::underlying_type_t<T> enum_to_value(T v) {
        return static_cast<std::underlying_type_t<T>>(v);
    }

    template <typename T>
    struct enable_enum_add_operator {};

    template <typename T, typename = std::enable_if_t<enable_enum_add_operator<T>::value>>
    constexpr T operator+(T a, std::underlying_type_t<T> b) {
        return static_cast<T>(enum_to_value(a) + b);
    }

    template <typename T, typename = std::enable_if_t<enable_enum_add_operator<T>::value>>
    constexpr T operator+(std::underlying_type_t<T> a, T b) {
        return static_cast<T>(a + enum_to_value(b));
    }
} // namespace utils

#endif // !CORE_UTILS_ENUM_HPP_INCLUDE
