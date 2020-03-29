#ifndef CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine {
    template <u64 size, u64 alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton_engine

#endif // !CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
