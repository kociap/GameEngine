#ifndef CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE

#include <cstdint>
template <uint64_t size, uint64_t alignment>
struct Aligned_Buffer {
    alignas(alignment) char buffer[size];
};

#endif // !CORE_MEMORY_ALIGNED_BUFFER_HPP_INCLUDE
