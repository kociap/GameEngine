#ifndef CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE
#define CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE

#include <core/atl/aligned_buffer.hpp>
#include <core/atl/memory.hpp>

namespace anton_engine {
    template<typename T>
    class Stack_Allocate {
    public:
        T& reference() {
            return *pointer();
        }

        T* pointer() {
            return reinterpret_cast<T*>(atl::launder(&buffer));
        }

        atl::Aligned_Buffer<sizeof(T), alignof(T)> buffer;
    };
} // namespace anton_engine

#endif // !CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE
