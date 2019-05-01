#ifndef CORE_CONTAINERS_VECTOR_HPP_INCLUDE
#define CORE_CONTAINERS_VECTOR_HPP_INCLUDE

#include "iterators.hpp"
#include "serialization.hpp"
#include <stdexcept>
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
        reference at(size_type index);
        const_reference at(size_type) const;
        reference at_unchecked(size_type);
        const_reference at_unchecked(size_type) const;
        // Returns a reference to the object which will be invalidated after reallocation
        reference operator[](size_type);
        const_reference operator[](size_type) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        pointer data();
        const_pointer data() const;

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

        bool empty() const;
        size_type size() const;
        size_type capacity() const;

        void resize(size_type n);
        void resize(size_type n, value_type const&);
        void reserve(size_type n);
        void set_capacity(size_type n);
        void shrink_to_fit();

        void push_back(value_type const&);
        void push_back(value_type&&);
        template <typename... CtorArgs>
        reference emplace_back(CtorArgs&&... args);
        void erase_unsorted(size_type index);
        void erase_unsorted_unchecked(size_type index);
        void pop_back();
        void clear();

    private:
        Allocator allocator;
        size_type _capacity = 64;
        size_type _size = 0;
        T* storage = nullptr;

    private:
        void attempt_copy(T* from, T* to);
        void attempt_move(T* from, T* to);
        template <typename... Ctor_Args>
        void attempt_construct(T* in, Ctor_Args&&... args);

        T* get_ptr(size_type index = 0) const;

        void move_contents(T* const& from, T* const& to, size_type number_of_elements_to_copy);
        void grow(size_type new_capacity);
        void shrink(size_type new_capacity);
        void check_size();

        friend void serialization::deserialize(Vector<T, Allocator>&, std::ifstream&);
    };
} // namespace containers

namespace serialization {
    template <typename T, typename Allocator>
    void serialize(containers::Vector<T, Allocator> const&, std::ofstream&);
    template <typename T, typename Allocator>
    void deserialize(containers::Vector<T, Allocator>&, std::ifstream&);
} // namespace serialization

#include "vector.tpp"

#endif // !CORE_CONTAINERS_POOL_HPP_INCLUDE
