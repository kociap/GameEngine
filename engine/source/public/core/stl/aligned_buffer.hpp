#ifndef CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine::anton_stl {
    template <usize size, usize alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_ALIGNED_BUFFER_HPP_INCLUDE
