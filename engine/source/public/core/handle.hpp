#ifndef CORE_HANDLE_HPP_INCLUDE
#define CORE_HANDLE_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
    template <typename T>
    class Handle {
    public:
        u64 value = static_cast<u64>(-1);
    };

    template <typename T>
    [[nodiscard]] constexpr bool operator==(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value == rhs.value;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator!=(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value != rhs.value;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator<(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value < rhs.value;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator>(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value > rhs.value;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator<=(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value <= rhs.value;
    }

    template <typename T>
    [[nodiscard]] constexpr bool operator>=(Handle<T> lhs, Handle<T> rhs) {
        return lhs.value >= rhs.value;
    }
} // namespace anton_engine

#endif //! CORE_HANDLE_HPP_INCLUDE
