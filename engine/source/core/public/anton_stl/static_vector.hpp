#ifndef CORE_ANTON_STL_STATIC_VECTOR_HPP_INCLUDE
#define CORE_ANTON_STL_STATIC_VECTOR_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/memory.hpp>
#include <cstdint>
#include <memory/aligned_buffer.hpp>
#include <memory/stack_allocate.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>
#include <stdexcept>
#include <type_traits>

namespace anton_engine::anton_stl {
    template <typename T, anton_stl::ssize_t Capacity>
    class Static_Vector {
        static_assert(Capacity >= 0, "Static_Vector's capacity may not be less than 0");

    public:
        using value_type = T;
        using allocator_type = void; // TODO add allocator?
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = anton_stl::Reverse_Iterator<iterator>;
        using reverse_const_iterator = anton_stl::Reverse_Iterator<const_iterator>;

        [[nodiscard]] reference operator[](size_type index);
        [[nodiscard]] const_reference operator[](size_type index) const;
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
        [[nodiscard]] reverse_const_iterator rbegin() const;
        [[nodiscard]] reverse_const_iterator rend() const;
        [[nodiscard]] reverse_const_iterator crbegin() const;
        [[nodiscard]] reverse_const_iterator crend() const;

        [[nodiscard]] constexpr size_type size() const;
        [[nodiscard]] constexpr size_type max_size() const;
        [[nodiscard]] constexpr size_type capacity() const;

        void resize(size_type count);
        void resize(size_type count, T const& value);
        void clear();

        iterator insert(const_iterator position, const value_type& value);
        iterator insert(const_iterator position, size_type n, const value_type& value);
        iterator insert(const_iterator position, value_type&& value);
        iterator insert(const_iterator position, std::initializer_list<value_type> ilist);
        template <typename Iterator>
        iterator insert(const_iterator position, Iterator first, Iterator last);

        template <typename... Args>
        iterator emplace(const_iterator position, Args&&... args);
        template <typename... Args>
        reference emplace_back(Args&&... args);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);
        iterator erase_unsorted(const_iterator position);
        iterator erase(size_type index);
        iterator erase_unsorted(size_type index);
        iterator erase_unsorted_unchecked(size_type index);

        void push_back(T const& val);
        void push_back(T&& val);

        void pop_back();

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> storage[Capacity];
        size_type _size = 0;

        T* get_ptr(size_type index = 0);
        T const* get_ptr_const(size_type index = 0) const;
        template <typename... Args>
        void attempt_construct(void* ptr, Args&&... args);
        void attempt_move(T* from, T* to);
        void check_size();

        friend void deserialize(serialization::Binary_Input_Archive&, anton_stl::Static_Vector<T, Capacity>&);
        friend void swap(Static_Vector<T, Capacity>&, Static_Vector<T, Capacity>&);
    };
} // namespace anton_engine::anton_stl

namespace anton_engine {
    template <typename T, uint64_t Capacity>
    void serialize(serialization::Binary_Output_Archive&, anton_stl::Static_Vector<T, Capacity> const&);
    template <typename T, uint64_t Capacity>
    void deserialize(serialization::Binary_Input_Archive&, anton_stl::Static_Vector<T, Capacity>&);
} // namespace anton_engine

#include <anton_stl/static_vector.tpp>

#endif // !CORE_ANTON_STL_STATIC_VECTOR_HPP_INCLUDE
