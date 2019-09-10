#ifndef CORE_ANTON_STL_MEMORY_COMMON_HPP_INCLUDE
#define CORE_ANTON_STL_MEMORY_COMMON_HPP_INCLUDE

namespace anton_engine::anton_stl {
    // addressof
    template <typename T>
    [[nodiscard]] inline constexpr T* addressof(T& value) ANTON_NOEXCEPT {
#ifndef ANTON_HAS_NO_BUILTIN_ADDRESSOF
        return __builtin_addressof(value);
#else
        // Note: We do not support volatile
        return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<char const&>(value)));
#endif
    }

    // Issue 2598 "addressof works on temporaries"
    // Deleted to prevent taking the address of temporaries
    template <typename T>
    T const* addressof(T const&&) = delete;
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_MEMORY_COMMON_HPP_INCLUDE
