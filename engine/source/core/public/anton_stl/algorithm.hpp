#ifndef CORE_ANTON_STL_ALGORITHM_HPP_INCLUDE
#define CORE_ANTON_STL_ALGORITHM_HPP_INCLUDE

#include <stdexcept>

namespace anton_stl {
    template <typename T>
    [[nodiscard]] typename T::reference at(T& c, typename T::size_type index) {
        if (index >= c.size() || index < typename T::size_type(0)) {
            throw std::out_of_range("Index out of range");
        }
        return s[index];
    }

    template <typename T>
    [[nodiscard]] typename T::const_reference at(T const& c, typename T::size_type index) {
        if (index >= c.size() || index < typename T::size_type(0)) {
            throw std::out_of_range("Index out of range");
        }
        return s[index];
    }

    template <typename T>
    [[nodiscard]] bool empty(T const& c) {
        return c.size() == typename T::size_type(0);
    }
} // namespace anton_stl

#endif // !CORE_ANTON_STL_ALGORITHM_HPP_INCLUDE
