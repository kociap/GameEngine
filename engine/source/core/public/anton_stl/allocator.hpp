#ifndef CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
#define CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <diagnostic_macros.hpp>

namespace anton_stl {
    // This allocator class is not templated on any type and provides malloc like
    // functionality to allocate properly aligned memory.
    class Allocator {
    public:
        // Allocates size bytes of memory
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment);
        void deallocate(void*, anton_stl::ssize_t size, anton_stl::ssize_t alignment);
    };

    [[nodiscard]] inline bool operator==(Allocator const&, Allocator const&) {
        return true; // All Allocators are stateless and may always be considered equal
    }

    [[nodiscard]] inline bool operator!=(Allocator const&, Allocator const&) {
        return false; // All Allocators are stateless and may always be considered equal
    }
} // namespace anton_stl

#endif // !CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
