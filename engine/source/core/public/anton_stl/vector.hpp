#ifndef CORE_ANTON_STL_VECTOR_HPP_INCLUDE
#define CORE_ANTON_STL_VECTOR_HPP_INCLUDE

#include <anton_stl/allocator.hpp>
#include <anton_stl/config.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/type_traits.hpp>
#include <anton_stl/utility.hpp>
#include <debug_macros.hpp>
#include <math/math.hpp>
#include <memory/memory.hpp>
#include <memory/stack_allocate.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>

#include <stdexcept> // includes std::allocator
#include <type_traits>

namespace anton_stl {
    template <typename T, typename Allocator = anton_stl::Allocator>
    class Vector {
        static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T> || std::is_trivially_copy_constructible_v<T>,
                      "Type is neither nothrow move constructible nor copy constructible");

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*; // Note:
        using const_iterator = T const*;
        using reverse_iterator = anton_stl::reverse_iterator<iterator>;
        using const_reverse_iterator = anton_stl::reverse_iterator<const_iterator>;

        /* [[nodiscard]] */ Vector();
        /* [[nodiscard]] */ explicit Vector(size_type size);
        /* [[nodiscard]] */ Vector(Reserve_Tag, size_type size);
        /* [[nodiscard]] */ Vector(size_type, value_type const&);
        /* [[nodiscard]] */ Vector(Vector const& original);
        /* [[nodiscard]] */ Vector(Vector&& from) noexcept;
        template <typename... Args>
        /* [[nodiscard]] */ Vector(Variadic_Construct_Tag, Args&&...);
        ~Vector();
        Vector& operator=(Vector const& original);
        Vector& operator=(Vector&& from) noexcept;

        [[nodiscard]] reference operator[](size_type);
        [[nodiscard]] const_reference operator[](size_type) const;
        [[nodiscard]] reference front();
        [[nodiscard]] const_reference front() const;
        [[nodiscard]] reference back();
        [[nodiscard]] const_reference back() const;
        [[nodiscard]] pointer data();
        [[nodiscard]] const_pointer data() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] const_iterator cend() const;
        [[nodiscard]] reverse_iterator rbegin();
        [[nodiscard]] reverse_iterator rend();
        [[nodiscard]] const_reverse_iterator crbegin() const;
        [[nodiscard]] const_reverse_iterator crend() const;

        [[nodiscard]] size_type size() const;
        [[nodiscard]] size_type capacity() const;

        void resize(size_type n);
        void resize(size_type n, value_type const&);
        void reserve(size_type n);
        void set_capacity(size_type n);
        void shrink_to_fit();

        template <typename Input_Iterator>
        void assign(Input_Iterator first, Input_Iterator last);
        void insert_unsorted(const_iterator position, value_type const& value);
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

        T* get_ptr(size_type index = 0);
        T const* get_ptr(size_type index = 0) const;
        T* get_iterator_underlying_ptr(iterator const&);
        T const* get_iterator_underlying_ptr(const_iterator const&) const;
        T* allocate(size_type);
        void deallocate(void*, size_type);

        void move_contents(T* const& from, T* const& to, size_type number_of_elements_to_copy);
        void grow(size_type new_capacity);
        void shrink(size_type new_capacity);
        void check_size();

        friend void serialization::deserialize(serialization::Binary_Input_Archive&, Vector<T, Allocator>&);
    };
} // namespace anton_stl

namespace serialization {
    template <typename T, typename Allocator>
    void serialize(Binary_Output_Archive&, anton_stl::Vector<T, Allocator> const&);
    template <typename T, typename Allocator>
    void deserialize(Binary_Input_Archive&, anton_stl::Vector<T, Allocator>&);
} // namespace serialization

#include <anton_stl/vector.tpp>

#endif // !CORE_ANTON_STL_VECTOR_HPP_INCLUDE
