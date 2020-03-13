#ifndef CORE_ATL_ALIGNED_BUFFER_HPP_INCLUDE
#define CORE_ATL_ALIGNED_BUFFER_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine::atl {
    template <usize size, usize alignment>
    struct Aligned_Buffer {
        alignas(alignment) char buffer[size];
    };
} // namespace anton_engine::atl

#endif // !CORE_ATL_ALIGNED_BUFFER_HPP_INCLUDE
