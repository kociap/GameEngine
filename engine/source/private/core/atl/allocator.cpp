#include <core/atl/allocator.hpp>

#include <core/atl/detail/utility_common.hpp>
#include <core/math/math.hpp>
#include <new>

namespace anton_engine::atl {
    static Allocator default_allocator;

    Memory_Allocator* get_default_allocator() {
        return &default_allocator;
    }

    // Allocator

    constexpr bool is_overaligned_for_new(isize alignment) {
        return alignment > static_cast<isize>(__STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }

    void* Allocator::allocate(isize const bytes, isize const alignment) {
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

    void Allocator::deallocate(void* ptr, isize const bytes, isize const alignment) {
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
        return this == &other;
    }

    // Buffer Allocator

    static char* adjust_to_alignment(char* address, usize alignment) {
        usize misalignment = (reinterpret_cast<usize>(address) & (alignment - 1));
        address += (misalignment != 0 ? alignment - misalignment : 0);
        return address;
    }

    // Buffer_Allocator::Buffer_Allocator(char* b, char* e): begin(adjust_to_alignment(b, alignof(Block_Data))), end(e) {
    //     Block_Data* block_data = reinterpret_cast<Block_Data*>(begin);
    //     *block_data = Block_Data{};
    // }

    // void* Buffer_Allocator::allocate(isize size, isize alignment) {
    //     auto aligned_block_size = [](Block_Data* block_begin, Block_Data* block_end, usize alignment) {
    //         char* begin = adjust_to_alignment(reinterpret_cast<char*>(block_begin + 1), alignment);
    //         char* end = reinterpret_cast<char*>(block_end);
    //         return end - begin;
    //     };
    // }

    // void Buffer_Allocator::deallocate(void*, isize size, isize alignment);
    // bool Buffer_Allocator::is_equal(Memory_Allocator const& other) const;

    // Polymorphic Allocator

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

    void* Polymorphic_Allocator::allocate(isize size, isize alignment) {
        return allocator->allocate(size, alignment);
    }

    void Polymorphic_Allocator::deallocate(void* mem, isize size, isize alignment) {
        allocator->deallocate(mem, size, alignment);
    }

    Memory_Allocator* Polymorphic_Allocator::get_wrapped_allocator() {
        return allocator;
    }

    Memory_Allocator const* Polymorphic_Allocator::get_wrapped_allocator() const {
        return allocator;
    }

    void swap(Polymorphic_Allocator& allocator1, Polymorphic_Allocator& allocator2) {
        atl::swap(allocator1.allocator, allocator2.allocator);
    }

    bool operator==(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }

    bool operator!=(Polymorphic_Allocator const& lhs, Polymorphic_Allocator const& rhs) {
        return *lhs.get_wrapped_allocator() == *rhs.get_wrapped_allocator();
    }
} // namespace anton_engine::atl
