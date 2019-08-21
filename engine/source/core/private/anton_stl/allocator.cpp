#include <anton_stl/allocator.hpp>
#include <chrono>
#include <math/math.hpp>
#include <new>
#include <stdexcept>

namespace anton_stl {
    constexpr bool is_overaligned_for_new(anton_stl::ssize_t alignment) {
        return alignment > static_cast<anton_stl::ssize_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }

    void* Allocator::allocate(anton_stl::ssize_t const bytes, anton_stl::ssize_t const alignment) {
        if (bytes <= 0 || alignment <= 0) {
            return nullptr;
        }

        // TODO: new aligns to at least __STDCPP_DEFAULT_NEW_ALIGNMENT__ which might not be suitable for all use cases
        // TODO: MSVC boost the alignment to 32 bytes for allocations that are > 4096 bytes. Why?
        if (is_overaligned_for_new(alignment)) {
            return ::operator new(static_cast<std::size_t>(bytes), static_cast<std::align_val_t>(alignment));
        } else {
            return ::operator new(static_cast<std::size_t>(bytes));
        }
    }

    void Allocator::deallocate(void* ptr, anton_stl::ssize_t const bytes, anton_stl::ssize_t const alignment) {
        if (bytes <= 0 || alignment <= 0) {
            return; // TODO add some sort of diagnostic maybe?
        }

        if (is_overaligned_for_new(alignment)) {
            ::operator delete(ptr, static_cast<std::size_t>(bytes), static_cast<std::align_val_t>(alignment));
        } else {
            ::operator delete(ptr, static_cast<std::size_t>(bytes));
        }
    }
} // namespace anton_stl
