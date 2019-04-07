#ifndef CORE_CONTAINERS_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_CONTAINERS_ALIGNED_BUFFER_HPP_INCLUDE

#include <cstdint>
namespace containers {
    template <uint64_t size, uint64_t alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace containers

#endif // !CORE_CONTAINERS_ALIGNED_BUFFER_HPP_INCLUDE
