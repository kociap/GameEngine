#ifndef CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE

#include <anton_stl/config.hpp>

namespace anton_stl {
    template <anton_stl::size_t size, anton_stl::size_t alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton_stl

#endif // !CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE
