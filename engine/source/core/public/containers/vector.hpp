#ifndef CORE_CONTAINERS_VECTOR_HPP_INCLUDE
#define CORE_CONTAINERS_VECTOR_HPP_INCLUDE

#include "iterators.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace containers {
    template <typename T, typename Allocator = std::allocator<T>>
    class Vector {
        static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T>,
                      "Type is neither nothrow move constructible nor copy constructible");

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = uint64_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = iterators::iterator<Vector>;
        using const_iterator = iterators::const_iterator<Vector>;
        using reverse_iterator = iterators::reverse_iterator<iterator>;
        using const_reverse_iterator = iterators::reverse_iterator<const_iterator>;

    public:
        Vector();
        // Allocates storage for cap elements
        // Does not construct elements
        explicit Vector(size_type cap);
        Vector(Vector const& original);
        Vector(Vector&& from) noexcept;
        Vector& operator=(Vector const& original);
        Vector& operator=(Vector&& from) noexcept;
        ~Vector();

    public:
        void resize(size_type n);
        void reserve(size_type n);
        // Destroys all elements
        // Doesn't shrink the container
        void clear();

        size_type size() const;
        size_type capacity() const;

        pointer data();
        const_pointer data() const;

        template <typename... CtorArgs>
        reference emplace_back(CtorArgs&&... args);

        void erase_unsorted(size_type index);
        void erase_unsorted_unchecked(size_type index);

        // Returns a reference to the object which will be invalidated after reallocation
        reference operator[](size_type);
        const_reference operator[](size_type) const;
        reference at(size_type index);
        const_reference at(size_type) const;
        reference at_unchecked(size_type);
        const_reference at_unchecked(size_type) const;

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        reverse_iterator rbegin();
        reverse_iterator rend();
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

    private:
        class Deleter {
        private:
            Allocator* allocator;
            size_type capacity;

        public:
            Deleter(Allocator& alloc, size_type cap) : allocator(&alloc), capacity(cap) {}
            Deleter(Deleter&) = default;
            Deleter(Deleter&&) = default;
            Deleter& operator=(Deleter& d) = default;
            Deleter& operator=(Deleter&&) = default;
            ~Deleter() {}

            void operator()(T* ptr) { allocator->deallocate(ptr, capacity); }
        };

        using storage_ptr = std::unique_ptr<T, Deleter>;

    private:
        Allocator allocator;
        size_type _capacity = 64;
        size_type _size = 0;
        storage_ptr storage;

    private:
        void attempt_construct(T* from, T* to);
        void attempt_move(T* from, T* to);

        // Reallocates current storage
        // Uses move constructor if T is nothrow move constructible
        // Otherwise copies all elements
        void reallocate(storage_ptr const& from, storage_ptr const& to);

        void check_size();
    };
} // namespace containers

#include "vector.tpp"

#endif // !CORE_CONTAINERS_POOL_HPP_INCLUDE
