#ifndef CORE_UTILITY_HPP_INCLUDE
#define CORE_UTILITY_HPP_INCLUDE

#include <type_traits>

template <typename T>
void swap(T& a, T& b) {
    T temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

#endif // !CORE_UTILITY_HPP_INCLUDE