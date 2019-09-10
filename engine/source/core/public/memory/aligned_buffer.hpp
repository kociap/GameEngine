#ifndef CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE

#include <cstdint>

namespace anton_engine {
    template <uint64_t size, uint64_t alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton_engine

#endif // !CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
