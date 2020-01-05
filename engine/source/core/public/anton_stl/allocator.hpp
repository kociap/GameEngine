#ifndef CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
#define CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/aligned_buffer.hpp>
#include <diagnostic_macros.hpp>

namespace anton_engine::anton_stl {
    // Memory_Allocator
    // An abstract class that provides an interface for all allocators that are supposed
    // to be used with Polymorphic_Allocator in polymorphic containers.
    //
    class Memory_Allocator {
    public:
        virtual ~Memory_Allocator() = default;

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR virtual void* allocate(isize size, isize alignment) = 0;
        virtual void deallocate(void*, isize size, isize alignment) = 0;
        [[nodiscard]] virtual bool is_equal(Memory_Allocator const&) const = 0;
    };

    [[nodiscard]] inline bool operator==(Memory_Allocator const& lhs, Memory_Allocator const& rhs) {
        return lhs.is_equal(rhs);
    }

    [[nodiscard]] inline bool operator!=(Memory_Allocator const& lhs, Memory_Allocator const& rhs) {
        return !(lhs == rhs);
    }

    // get_default_allocator
    //
    [[nodiscard]] Memory_Allocator* get_default_allocator();

    // Allocator
    // A generic allocator that provides malloc like functionality to allocate properly aligned memory.
    // This allocator class is not templated on any type and inherits from Memory_Allocator
    // to allow it to be used in polymorphic containers.
    //
    class Allocator: public Memory_Allocator {
    public:
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment) override;
        void deallocate(void*, isize size, isize alignment) override;
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

    // // Buffer_Allocator
    // class Buffer_Allocator: public Memory_Allocator {
    // private:
    //     struct Block_Data {
    //         Block_Data* previous_block = nullptr;
    //         Block_Data* next_block = nullptr;
    //         bool free = true;
    //     };

    // public:
    //     static constexpr usize data_block_size = sizeof(Block_Data);

    //     Buffer_Allocator(char*, char*);

    //     [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment) override;
    //     void deallocate(void*, isize size, isize alignment) override;
    //     [[nodiscard]] bool is_equal(Memory_Allocator const& other) const override;

    // private:
    //     // Store the linked list of blocks in a separate array
    //     char* begin;
    //     char* end;
    // };

    // Stack_Allocator
    // The effective size of the buffer is smaller due to.
    //
    template <usize size, usize alignment>
    class Stack_Allocator: public Memory_Allocator {
    public:
        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment) override {}

        void deallocate(void*, isize size, isize alignment) override {}

        [[nodiscard]] bool is_equal(Memory_Allocator const& other) const override {
            return false; // TODO: A way to compare allocators.
        }

    private:
        struct Block_Data {
            void* previous_block;
            void* next_block;
            bool free;
        };

        Aligned_Buffer<size, alignment> stack;
    };

    template <usize S1, usize A1, usize S2, usize A2>
    [[nodiscard]] constexpr bool operator==(Stack_Allocator<S1, A1> const&, Stack_Allocator<S2, A2> const&) {
        return S1 == S2 && A1 == A2;
    }

    template <usize S1, usize A1, usize S2, usize A2>
    [[nodiscard]] constexpr bool operator!=(Stack_Allocator<S1, A1> const&, Stack_Allocator<S2, A2> const&) {
        return S1 != S2 || A1 != A2;
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

        [[nodiscard]] ANTON_DECLSPEC_ALLOCATOR void* allocate(isize size, isize alignment);
        void deallocate(void*, isize size, isize alignment);

        Memory_Allocator* get_wrapped_allocator();
        Memory_Allocator const* get_wrapped_allocator() const;

        friend void swap(Polymorphic_Allocator&, Polymorphic_Allocator&);

    private:
        Memory_Allocator* allocator;
    };

    [[nodiscard]] bool operator==(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
    [[nodiscard]] bool operator!=(Polymorphic_Allocator const&, Polymorphic_Allocator const&);
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_ALLOCATOR_HPP_INCLUDE
