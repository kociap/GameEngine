#ifndef CORE_UTILS_ENUM_HPP_INCLUDE
#define CORE_UTILS_ENUM_HPP_INCLUDE

#include <type_traits>

namespace anton_engine::utils {
    template <typename T>
    constexpr std::underlying_type_t<T> enum_to_value(T v) {
        return static_cast<std::underlying_type_t<T>>(v);
    }

    template <typename T>
    struct enable_enum_add_operator {};

    template <typename T>
    struct enable_enum_bitwise_and {};

    template <typename T>
    struct enable_enum_bitwise_or {};

} // namespace anton_engine::utils

template <typename T, bool = anton_engine::utils::enable_enum_add_operator<T>::value>
constexpr T operator+(T a, std::underlying_type_t<T> b) {
    return static_cast<T>(anton_engine::utils::enum_to_value(a) + b);
}

template <typename T, bool = anton_engine::utils::enable_enum_add_operator<T>::value>
constexpr T operator+(std::underlying_type_t<T> a, T b) {
    return static_cast<T>(a + anton_engine::utils::enum_to_value(b));
}

template <typename T, bool = anton_engine::utils::enable_enum_bitwise_and<T>::value>
constexpr T operator&(T a, T b) {
    return static_cast<T>(anton_engine::utils::enum_to_value(a) & anton_engine::utils::enum_to_value(b));
}

template <typename T, bool = anton_engine::utils::enable_enum_bitwise_or<T>::value>
constexpr T operator|(T const a, T const b) {
    return static_cast<T>(anton_engine::utils::enum_to_value(a) | anton_engine::utils::enum_to_value(b));
}

#endif // !CORE_UTILS_ENUM_HPP_INCLUDE
