#ifndef CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE
#define CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE

#include "memory/aligned_buffer.hpp"

template <typename T>
class Stack_Allocate {
public:
    T& reference() {
        return *pointer();
    }

    T* pointer() {
        return reinterpret_cast<T*>(std::launder(&buffer));
    }

    Aligned_Buffer<sizeof(T), alignof(T)> buffer;
};

#endif // !CORE_MEMORY_STACK_ALLOCATE_HPP_INCLUDE
