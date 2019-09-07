#include <anton_stl/allocator.hpp>

#include <anton_stl/detail/utility_common.hpp>
#include <math/math.hpp>
#include <new>
#include <typeinfo>

namespace anton_stl {
    static Allocator default_allocator;

    Memory_Allocator* get_default_allocator() {
        return &default_allocator;
    }

    constexpr bool is_overaligned_for_new(anton_stl::ssize_t alignment) {
        return alignment > static_cast<anton_stl::ssize_t>(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }

    void* Allocator::allocate(anton_stl::ssize_t const bytes, anton_stl::ssize_t const alignment) {
        if (bytes <= 0 || alignment <= 0) {
            return nullptr;
        }

        // TODO: new aligns to at least __STDCPP_DEFAULT_NEW_ALIGNMENT__ which might not be suitable for all use cases
        // TODO: MSVC boosts the alignment to 32 bytes for allocations that are > 4096 bytes. Why?
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

    bool Allocator::is_equal(Memory_Allocator const& other) const {
        return this == &other || typeid(*this) == typeid(other);
    }

    Polymorphic_Allocator::Polymorphic_Allocator(): allocator(get_default_allocator()) {}

    Polymorphic_Allocator::Polymorphic_Allocator(Polymorphic_Allocator const& other): allocator(other.allocator) {}

    Polymorphic_Allocator::Polymorphic_Allocator(Polymorphic_Allocator&& other) noexcept: allocator(other.allocator) {
        other.allocator = nullptr;
    }

    Polymorphic_Allocator::Polymorphic_Allocator(Memory_Allocator* new_allocator): allocator(new_allocator) {}

    Polymorphic_Allocator& Polymorphic_Allocator::operator=(Polymorphic_Allocator&& other) noexcept {
        allocator = other.allocator;
        other.allocator = nullptr;
        return *this;
    }

    void* Polymorphic_Allocator::allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment) {
        return allocator->allocate(size, alignment);
    }

    void Polymorphic_Allocator::deallocate(void* mem, anton_stl::ssize_t size, anton_stl::ssize_t alignment) {
        allocator->deallocate(mem, size, alignment);
    }

    Memory_Allocator* Polymorphic_Allocator::get_wrapped_allocator() {
        return allocator;
    }

    Memory_Allocator const* Polymorphic_Allocator::get_wrapped_allocator() const {
        return allocator;
    }

    void swap(Polymorphic_Allocator& allocator1, Polymorphic_Allocator& allocator2) {
        anton_stl::swap(allocator1.allocator, allocator2.allocator);
    }

    bool operator==(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }

    bool operator!=(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }
} // namespace anton_stl
