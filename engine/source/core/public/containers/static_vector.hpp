#ifndef CORE_CONTAINERS_STATIC_VECTOR_HPP_INCLUDE
#define CORE_CONTAINERS_STATIC_VECTOR_HPP_INCLUDE

#include "iterators.hpp"
#include "memory/aligned_buffer.hpp"
#include "serialization.hpp"
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace containers {
    template <typename T, uint64_t _capacity>
    class Static_Vector {
    public:
        using value_type = T;
        using allocator_type = void; // TODO add allocator?
        using size_type = uint64_t;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = iterators::iterator<Static_Vector>;
        using const_iterator = iterators::const_iterator<Static_Vector>;
        using reverse_iterator = iterators::reverse_iterator<iterator>;
        using const_reverse_iterator = iterators::reverse_iterator<const_iterator>;

        reference at(size_type index);
        const_reference at(size_type index) const;
        reference at_unchecked(size_type index);
        const_reference at_unchecked(size_type index) const;
        reference operator[](size_type index);
        const_reference operator[](size_type index) const;

        reference front();
        const_reference front() const;

        reference back();
        const_reference back() const;

        pointer data();
        const_pointer data() const;

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        const_reverse_iterator crbegin() const;

        reverse_iterator rend();
        const_reverse_iterator rend() const;
        const_reverse_iterator crend() const;

        bool empty() const;
        constexpr size_type size() const;
        constexpr size_type max_size() const;
        constexpr size_type capacity() const;

        void resize(size_type count, T const& value);
        void resize(size_type count);
        void reserve(size_type new_cap);
        void shrink_to_fit();
        void clear();

        iterator insert(const_iterator position, const value_type& value);
        iterator insert(const_iterator position, size_type n, const value_type& value);
        iterator insert(const_iterator position, value_type&& value);
        iterator insert(const_iterator position, std::initializer_list<value_type> ilist);
        template <typename Iterator>
        iterator insert(const_iterator position, Iterator first, Iterator last);

        template <typename... Args>
        iterator emplace(const_iterator position, Args&&... args);

        iterator erase(const_iterator position);
        iterator erase(const_iterator first, const_iterator last);
        iterator erase_unsorted(const_iterator position);
        iterator erase(size_type index);
        iterator erase_unsorted(size_type index);
        iterator erase_unsorted_unchecked(size_type index);

        void push_back(T const& val);
        void push_back(T&& val);

        template <typename... Args>
        reference emplace_back(Args&&... args);

        void pop_back();

        void swap(Static_Vector& vec) {
            using std::swap;
            swap(storage, vec.storage);
            swap(_size, vec._size);
        }

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> storage[_capacity];
        size_type _size = 0;

        T* get_ptr(size_type index = 0);
        T const* get_ptr_const(size_type index = 0) const;
        template <typename... Args>
        void attempt_construct(void* ptr, Args&&... args);
        void attempt_move(T* from, T* to);
        void check_size();

        template <typename T, uint64_t _capacity>
        friend void deserialize(containers::Static_Vector<T, _capacity>& vec, std::ifstream& in);
    };

    template <typename T, uint64_t _capacity>
    void swap(Static_Vector<T, _capacity>&, Static_Vector<T, _capacity>&);
} // namespace containers

namespace serialization {
    template <typename T, uint64_t _capacity>
    void serialize(std::ofstream& out, containers::Static_Vector<T, _capacity> const& vec);
    template <typename T, uint64_t _capacity>
    void deserialize(containers::Static_Vector<T, _capacity>& vec, std::ifstream& in);
} // namespace serialization

#include "static_vector.tpp"

#endif // !CORE_CONTAINERS_STATIC_VECTOR_HPP_INCLUDE
