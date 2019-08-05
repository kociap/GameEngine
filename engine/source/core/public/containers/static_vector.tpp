#include "memory/memory.hpp"

namespace containers {
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::at(size_type index) {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }

        return at_unchecked(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::at(size_type index) const {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }

        return at_unchecked(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::at_unchecked(size_type index) {
        return *get_ptr(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::at_unchecked(size_type index) const {
        return *get_ptr_const(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::operator[](size_type index) {
        return at_unchecked(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::operator[](size_type index) const {
        return at_unchecked(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::front() {
        return at_unchecked(0);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::front() const {
        return at_unchecked(0);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::back() {
        return at_unchecked(_size - 1);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::back() const {
        return at_unchecked(_size - 1);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::pointer Static_Vector<T, _capacity>::data() {
        return get_ptr(0);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_pointer Static_Vector<T, _capacity>::data() const {
        return get_ptr_const(0);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::begin() {
        return iterator(get_ptr(0));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::begin() const {
        return const_iterator(get_ptr(0));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::cbegin() const {
        return const_iterator(get_ptr(0));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::end() {
        return iterator(get_ptr(_size));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::end() const {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::cend() const {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reverse_iterator Static_Vector<T, _capacity>::rbegin() {
        return reverse_iterator(end());
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reverse_iterator Static_Vector<T, _capacity>::rbegin() const {
        return const_reverse_iterator(end());
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reverse_iterator Static_Vector<T, _capacity>::crbegin() const {
        return const_reverse_iterator(cend());
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::reverse_iterator Static_Vector<T, _capacity>::rend() {
        return reverse_iterator(begin());
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reverse_iterator Static_Vector<T, _capacity>::rend() const {
        return const_reverse_iterator(begin());
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reverse_iterator Static_Vector<T, _capacity>::crend() const {
        return const_reverse_iterator(cbegin());
    }

    template <typename T, uint64_t _capacity>
    constexpr typename Static_Vector<T, _capacity>::size_type Static_Vector<T, _capacity>::size() const {
        return _size;
    }

    template <typename T, uint64_t _capacity>
    constexpr typename Static_Vector<T, _capacity>::size_type Static_Vector<T, _capacity>::max_size() const {
        return _capacity;
    }

    template <typename T, uint64_t _capacity>
    constexpr typename Static_Vector<T, _capacity>::size_type Static_Vector<T, _capacity>::capacity() const {
        return _capacity;
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::resize(size_type n, T const& value) {
        if (n > _capacity) {
            throw std::out_of_range("Attempt to construct more than capacity() elements in Static_Vector");
        }

        if (n > _size) { // Upsizing is the more likely case
            memory::uninitialized_fill(get_ptr(_size), get_ptr(n), value);
            _size = n;
        } else {
            memory::destruct(get_ptr(n), get_ptr(_size));
            _size = n;
        }
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::resize(size_type n) {
        if (n > _capacity) {
            throw std::out_of_range("Attempt to construct more than capacity() elements in Static_Vector");
        }

        if (n > _size) { // Upsizing is the more likely case
            memory::uninitialized_default_construct(get_ptr(_size), get_ptr(n));
            _size = n;
        } else {
            memory::destruct(get_ptr(n), get_ptr(_size));
            _size = n;
        }
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::clear() {
        memory::destruct_n(get_ptr(), _size);
        _size = 0;
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, const value_type& value) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, size_type n, const value_type& value) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, value_type&& value) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, std::initializer_list<value_type> ilist) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    template <typename Iterator>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, Iterator first, Iterator last) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    template <typename... Args>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::emplace(const_iterator position, Args&&... args) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(const_iterator position) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(const_iterator first, const_iterator last) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase_unsorted(const_iterator position) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(size_type index) {
        return begin(); // TODO implement
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase_unsorted(size_type index) {
        if (index > _size) {
            throw std::out_of_range("Index out of range");
        }

        return erase_unsorted_unchecked(index);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase_unsorted_unchecked(size_type index) {
        T* elem = get_ptr(index);
        T* last_elem = get_ptr(_size - 1);
        *elem = std::move(*last_elem);
        --_size;
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::push_back(T const& val) {
        check_size();
        attempt_construct(get_ptr(_size), val);
        ++size;
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::push_back(T&& val) {
        check_size();
        attempt_construct(get_ptr(_size), std::move(val));
        ++_size;
    }

    template <typename T, uint64_t _capacity>
    template <typename... Args>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::emplace_back(Args&&... args) {
        check_size();
        attempt_construct(get_ptr(_size), std::forward<Args>(args)...);
        return at_unchecked(_size++);
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::pop_back() {
        memory::destruct(get_ptr(_size - 1));
        --_size;
    }

    template <typename T, uint64_t _capacity>
    T* Static_Vector<T, _capacity>::get_ptr(size_type index) {
        return reinterpret_cast<T*>(std::launder(&storage[index]));
    }

    template <typename T, uint64_t _capacity>
    T const* Static_Vector<T, _capacity>::get_ptr_const(size_type index) const {
        return reinterpret_cast<T const*>(std::launder(&storage[index]));
    }

    template <typename T, uint64_t _capacity>
    template <typename... Args>
    void Static_Vector<T, _capacity>::attempt_construct(void* ptr, Args&&... args) {
        if constexpr (std::is_constructible_v<T, Args&&...>) {
            ::new (ptr) T(std::forward<Args>(args)...);
        } else {
            ::new (ptr) T{std::forward<Args>(args)...};
        }
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::attempt_move(T* from, T* to) {
        ::new (to) T(std::move(*from));
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::check_size() {
        if (_size == _capacity) {
            throw std::length_error("Attempt to construct more than capacity() elements in Static_Vector");
        }
    }
} // namespace containers

namespace serialization {
    template <typename T, uint64_t _capacity>
    void serialize(Binary_Output_Archive& out, containers::Static_Vector<T, _capacity> const& vec) {
        out.write(vec.size());
        for (T const& elem: vec) {
            serialization::serialize(out, elem);
        }
    }

    template <typename T, uint64_t _capacity>
    void deserialize(Binary_Input_Archive& in, containers::Static_Vector<T, _capacity>& vec) {
        containers::Static_Vector<T, _capacity>::size_type size;
        in.read(_size);
        vec.clear();
        if constexpr (std::is_default_constructible_v<T>) {
            vec.resize(size);
            try {
                for (T& elem: vec) {
                    serialization::deserialize(in, elem);
                }
            } catch (...) {
                // TODO move stream backward
                memory::destruct_n(vec.get_ptr(), size);
                throw;
            }
        } else {
            size_type n = size;
            try {
                for (; n > 0; --n) {
                    Stack_Allocate<T> elem;
                    vec.push_back(std::move(elem.reference()));
                    serialization::deserialize(in, vec.back());
                }
                vec._size = size;
            } catch (...) {
                // TODO move stream backward
                memory::destruct_n(vec.get_ptr(), size - n);
                throw;
            }
        }
    }
} // namespace serialization