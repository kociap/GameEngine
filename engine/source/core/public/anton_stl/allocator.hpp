#ifndef CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
#define CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE

#include <anton_stl/aligned_buffer.hpp>
#include <anton_stl/config.hpp>
#include <diagnostic_macros.hpp>

namespace anton_stl {
    // Memory_Allocator
    // An abstract class that provides an interface for all allocators that are supposed
    // to be used with Polymorphic_Allocator in polymorphic containers.
    //
    class Memory_Allocator {
    public:
        virtual ~Memory_Allocator() = default;

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR virtual void* allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment) = 0;
        virtual void deallocate(void*, anton_stl::ssize_t size, anton_stl::ssize_t alignment) = 0;
        [[nodiscard]] virtual bool is_equal(Memory_Allocator const&) const = 0;
    };

    // get_default_allocator
    //
    Memory_Allocator* get_default_allocator();

    // Allocator
    // A generic allocator that provides malloc like functionality to allocate properly aligned memory.
    // This allocator class is not templated on any type and inherits from Memory_Allocator
    // to allow it to be used in polymorphic containers.
    //
    class Allocator: public Memory_Allocator {
    public:
        // Allocates size bytes of memory
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment) override;
        void deallocate(void*, anton_stl::ssize_t size, anton_stl::ssize_t alignment) override;
        [[nodiscard]] bool is_equal(Memory_Allocator const& other) const override;
    };

    inline void swap(Allocator&, Allocator&) {
        // Stateless allocator. No need to do anything.
    }

    [[nodiscard]] inline bool operator==(Allocator const&, Allocator const&) {
        return true; // All Allocators are stateless and may always be considered equal.
    }

    [[nodiscard]] inline bool operator!=(Allocator const&, Allocator const&) {
        return false; // All Allocators are stateless and may always be considered equal.
    }

    // Stack_Allocator
    template <anton_stl::size_t size, anton_stl::size_t alignment>
    class Stack_Allocator: public Memory_Allocator {
    public:
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment) override;
        void deallocate(void*, anton_stl::ssize_t size, anton_stl::ssize_t alignment) override;
        [[nodiscard]] bool is_equal(Memory_Allocator const& other) const override;

        friend void swap(Stack_Allocator&, Stack_Allocator&);

    private:
        Aligned_Buffer<size, alignment> stack;
    };

    template <anton_stl::size_t S1, anton_stl::size_t A1, anton_stl::size_t S2, anton_stl::size_t A2>
    [[nodiscard]] inline bool operator==(Stack_Allocator<S1, A1> const&, Stack_Allocator<S2, A2> const&) {
        return false;
    }

    template <anton_stl::size_t S1, anton_stl::size_t A1, anton_stl::size_t S2, anton_stl::size_t A2>
    [[nodiscard]] inline bool operator!=(Stack_Allocator<S1, A1> const&, Stack_Allocator<S2, A2> const&) {
        return true;
    }

    // Polymorphic_Allocator
    // A wrapper around Memory_Allocator to allow any custom allocator to be used with any
    // container without baking the allocator type into container type.
    //
    class Polymorphic_Allocator {
    public:
        Polymorphic_Allocator();
        Polymorphic_Allocator(Polymorphic_Allocator const&);
        Polymorphic_Allocator(Polymorphic_Allocator&&) noexcept;
        Polymorphic_Allocator(Memory_Allocator*);
        ~Polymorphic_Allocator() = default;

        Polymorphic_Allocator& operator=(Polymorphic_Allocator const&) = delete;
        Polymorphic_Allocator& operator=(Polymorphic_Allocator&&) noexcept;

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(anton_stl::ssize_t size, anton_stl::ssize_t alignment);
        void deallocate(void*, anton_stl::ssize_t size, anton_stl::ssize_t alignment);

        Memory_Allocator* get_wrapped_allocator();
        Memory_Allocator const* get_wrapped_allocator() const;

        friend void swap(Polymorphic_Allocator&, Polymorphic_Allocator&);

    private:
        Memory_Allocator* allocator;
    };

    [[nodiscard]] bool operator==(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
    [[nodiscard]] bool operator!=(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
} // namespace anton_stl

#endif // !CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
