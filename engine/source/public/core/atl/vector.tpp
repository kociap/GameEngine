#include <core/assert.hpp>

namespace anton_engine::atl {
    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector() {
        _data = allocate(_capacity);
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type const n) {
        _capacity = math::max(_capacity, n);
        _data = allocate(_capacity);
        try {
            atl::uninitialized_default_construct_n(_data, n);
            _size = n;
        } catch (...) {
            deallocate(_data, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(atl::Reserve_Tag, size_type const n): _capacity(n) {
        _data = allocate(_capacity);
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type n, value_type const& value) {
        _capacity = math::max(_capacity, n);
        _data = allocate(_capacity);
        try {
            atl::uninitialized_fill_n(_data, n, value);
            _size = n;
        } catch (...) {
            deallocate(_data, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector const& v): _allocator(v._allocator), _capacity(v._capacity) {
        _data = allocate(_capacity);
        try {
            atl::uninitialized_copy_n(v._data, v._size, _data);
            _size = v._size;
        } catch (...) {
            deallocate(_data, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector&& v) noexcept: _allocator(atl::move(v._allocator)), _capacity(v._capacity), _size(v._size), _data(v._data) {
        v._data = nullptr;
        v._capacity = 0;
        v._size = 0;
    }

    template<typename T, typename Allocator>
    template<typename Input_Iterator>
    Vector<T, Allocator>::Vector(Range_Construct_Tag, Input_Iterator first, Input_Iterator last) {
        size_type const count = last - first;
        _capacity = math::max(_capacity, count);
        _size = count;
        _data = allocate(_capacity);
        try {
            atl::uninitialized_copy(first, last, _data);
        } catch(...) {
            deallocate(_data, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    template <typename... Args>
    Vector<T, Allocator>::Vector(Variadic_Construct_Tag, Args&&... args) {
        _capacity = math::max(_capacity, static_cast<size_type>(sizeof...(Args)));
        _data = allocate(_capacity);
        try {
            atl::uninitialized_variadic_construct(_data, atl::forward<Args>(args)...);
            _size = static_cast<size_type>(sizeof...(Args));
        } catch (...) {
            deallocate(_data, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::~Vector() {
        if (_data != nullptr) {
            atl::destruct_n(_data, _size);
            deallocate(_data, _capacity);
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector const& v) {
        // TODO: Get rid of this and move to polymorphic
        // static_assert(std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value, "Allocator is not copy assignable");
        _allocator = v._allocator;
        T* new_storage = allocate(v._capacity);
        try {
            atl::uninitialized_copy_n(v._data, v._size, new_storage);
        } catch (...) {
            deallocate(new_storage, v._capacity);
            throw;
        }
        atl::destruct_n(_data, _size);
        // Note: assumes v._allocator == _allocator is true
        deallocate(_data, _capacity);
        _data = new_storage;
        _size = v._size;
        _capacity = v._capacity;
        return *this;
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& v) noexcept {
        // Note: We ignore the fact that the allocator_traits<Allocator>::propagate_on_container_swap might be false
        // or the allocators do not compare equal.
        swap(_data, v._data);
        swap(_allocator, v._allocator);
        swap(_capacity, v._capacity);
        swap(_size, v._size);
        return *this;
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::operator[](size_type index) -> reference {
        return *get_ptr(index);
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::operator[](size_type index) const -> const_reference {
        return *get_ptr(index);
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::data() -> pointer {
        return get_ptr();
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::data() const -> const_pointer {
        return get_ptr();
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::begin() -> iterator {
        return iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::end() -> iterator {
        return iterator(get_ptr(_size));
    }
    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::begin() const -> const_iterator {
        return const_iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::end() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::cbegin() const -> const_iterator {
        return const_iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::cend() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::size() const -> size_type {
        return _size;
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::capacity() const -> size_type {
        return _capacity;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type n, value_type const& value) {
        ensure_capacity(n);
        if (n > _size) {
            atl::uninitialized_fill(get_ptr(_size), get_ptr(n), value);
        } else {
            atl::destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type n) {
        ensure_capacity(n);
        if (n > _size) {
            atl::uninitialized_default_construct(get_ptr(_size), get_ptr(n));
        } else {
            atl::destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::reserve(size_type n) {
        ensure_capacity(n);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::set_capacity(size_type new_capacity) {
        if (new_capacity != _capacity) {
            T* new_data = allocate(new_capacity);
            try {
                // TODO: More reallocation options
                if constexpr (atl::is_move_constructible<T>) {
                    atl::uninitialized_move_n(_data, math::min(new_capacity, _size), new_data);
                } else {
                    atl::uninitialized_copy_n(_data, math::min(new_capacity, _size), new_data);
                }
            } catch (...) { deallocate(new_data, new_capacity); }
            destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
            _size = math::min(new_capacity, _size);
        }
    }

    template<typename T, typename Allocator>
    void Vector<T, Allocator>::force_size(size_type n) {
        ANTON_ASSERT(n <= _capacity, "Requested size is greater than capacity.");
        _size = n;
    }

    template <typename T, typename Allocator>
    template <typename Input_Iterator>
    void Vector<T, Allocator>::assign(Input_Iterator first, Input_Iterator last) {
        ensure_capacity(last - first);
        destruct_n(_data, _size);
        copy(first, last, _data);
        _size = last - first;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::insert(size_type const position, value_type const& value) {
        if constexpr (ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position <= _size && position >= 0, "Index out of bounds.");
        }

        if (_size == _capacity || position != _size) {
            if (_size != _capacity) {
                atl::move_backward(get_ptr(position), get_ptr(_size), get_ptr(_size + 1));
                attempt_construct(get_ptr(position), value);
                _size += 1;
            } else {
                i64 const new_capacity = _capacity * 2;
                T* const new_data = allocate(new_capacity);
                i64 moved = 0;
                try {
                    atl::move(get_ptr(0), get_ptr(position), new_data);
                    moved = position;
                    attempt_construct(new_data + position, value);
                    moved += 1;
                    atl::move(get_ptr(position), get_ptr(_size), new_data + moved);
                } catch (...) {
                    atl::destruct_n(new_data, moved);
                    deallocate(new_data, new_capacity);
                    throw;
                }
                atl::destruct_n(_data, _size);
                deallocate(_data, _capacity);
                _capacity = new_capacity;
                _data = new_data;
                _size += 1;
            }
        } else {
            // Quick path when position points to end and we have room for one more element.
            attempt_construct(get_ptr(_size), value);
            _size += 1;
        }
    }

    template <typename T, typename Allocator>
    template<typename Input_Iterator>
    void Vector<T, Allocator>::insert(size_type position, Input_Iterator first, Input_Iterator last) {
        if constexpr (ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position <= _size && position >= 0, "Index out of bounds.");
        }

        // TODO: Distance
        // TODO: Non-movable types.
        // TODO: Fix this thing because I copy-pasted the above which isn't 100% correct
        i64 const dist = last - first;
        if (_size + dist > _capacity || position != _size) {
            if (_size + dist <= _capacity) {
                atl::move_backward(get_ptr(position), get_ptr(_size), get_ptr(_size + dist));
                atl::copy(first, last, get_ptr(position));
                _size += dist;
            } else {
                i64 const new_capacity = _capacity * 2;
                T* const new_data = allocate(new_capacity);
                i64 moved = 0;
                try {
                    atl::move(get_ptr(0), get_ptr(position), new_data);
                    moved = position;
                    atl::copy(first, last, new_data + moved);
                    moved += dist;
                    atl::move(get_ptr(position), get_ptr(_size), new_data + moved);
                } catch (...) {
                    atl::destruct_n(new_data, moved);
                    deallocate(new_data, new_capacity);
                    throw;
                }
                atl::destruct_n(_data, _size);
                deallocate(_data, _capacity);
                _capacity = new_capacity;
                _data = new_data;
                _size += dist;
            }
        } else {
            // Quick path when position points to end and we have room for one more element.
            atl::copy(first, last, get_ptr(_size));
            _size += dist;
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::insert_unsorted(const_iterator position, value_type const& value) {
        if constexpr (ANTON_ITERATOR_DEBUG) {
            ANTON_FAIL(position < _size && position >= 0, "Index out of bounds.");
        }

        ensure_capacity(_size + 1);
        size_type offset = static_cast<size_type>(position - _data);
        if (offset == _size) {
            attempt_construct(get_ptr(offset), value);
            ++_size;
        } else {
            T* elem_ptr = get_ptr(offset);
            if constexpr (atl::is_move_constructible<T>) {
                attempt_move(elem_ptr, get_ptr(_size));
            } else {
                attempt_copy(elem_ptr, get_ptr(_size));
            }
            atl::destruct(elem_ptr);
            attempt_construct(elem_ptr, value);
            ++_size;
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(value_type const& val) {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, val);
        ++_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(value_type&& val) {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, atl::move(val));
        ++_size;
    }

    template <typename T, typename Allocator>
    template <typename... CtorArgs>
    auto Vector<T, Allocator>::emplace_back(CtorArgs&&... args) -> reference {
        ensure_capacity(_size + 1);
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, atl::forward<CtorArgs>(args)...);
        ++_size;
        return *elem_ptr;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted(size_type index) {
        if (index > _size) {
            throw std::out_of_range("index out of range");
        }

        erase_unsorted_unchecked(index);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted_unchecked(size_type index) {
        T* elem_ptr = get_ptr(index);
        T* last_elem_ptr = get_ptr(_size - 1);
        if (elem_ptr != last_elem_ptr) { // Prevent self move-assignment
            *elem_ptr = atl::move(*last_elem_ptr);
        }
        atl::destruct(last_elem_ptr);
        --_size;
    }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::erase_unsorted(const_iterator iter) -> iterator {
#if ANTON_ITERATOR_DEBUG

#endif // !ANTON_ITERATOR_DEBUG

        auto pos = const_cast<value_type*>(iter);
        if (iter != cend() - 1) {
            *pos = atl::move(*get_ptr(_size - 1));
        }
        atl::destruct(get_ptr(_size - 1));
        _size -= 1;
        return pos;
    }

    //     template <typename T, typename Allocator>
    //     auto Vector<T, Allocator>::erase_unsorted(const_iterator first, const_iterator last) -> iterator {
    // #if ANTON_ITERATOR_DEBUG

    // #endif // ANTON_ITERATOR_DEBUG

    //         if (first != last) {
    //             auto first_last_elems = last - first;
    //             auto last_end_elems = end() - last;
    //             auto elems_till_end = math::min(first_last_elems, last_end_elems);
    //             atl::move(end() - elems_till_end, end(), first);
    //             atl::destruct(end() - elems_till_end, end());
    //             _size -= first_last_elems;
    //         }

    //         return first;
    //     }

    template <typename T, typename Allocator>
    auto Vector<T, Allocator>::erase(const_iterator first, const_iterator last) -> iterator {
#if ANTON_ITERATOR_DEBUG

#endif // ANTON_ITERATOR_DEBUG

        if (first != last) {
            iterator pos = atl::move(const_cast<value_type*>(last), end(), const_cast<value_type*>(first));
            atl::destruct(pos, end());
            _size -= last - first;
        }

        return const_cast<value_type*>(first);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::pop_back() {
        ANTON_VERIFY(_size > 0, "Trying to pop an element from an empty Array.");
        T* last_elem_ptr = get_ptr(_size - 1);
        atl::destruct(last_elem_ptr);
        --_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::clear() {
        atl::destruct(get_ptr(), get_ptr(_size));
        _size = 0;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_copy(T* from, T* to) {
        ::new (to) T(*from);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_move(T* from, T* to) {
        ::new (to) T(atl::move(*from));
    }

    template <typename T, typename Allocator>
    template <typename... Ctor_Args>
    void Vector<T, Allocator>::attempt_construct(T* in, Ctor_Args&&... args) {
        if constexpr (atl::is_constructible<T, Ctor_Args&&...>) {
            ::new (in) T(atl::forward<Ctor_Args>(args)...);
        } else {
            ::new (in) T{atl::forward<Ctor_Args>(args)...};
        }
    }

    template <typename T, typename Allocator>
    T* Vector<T, Allocator>::get_ptr(size_type index) {
        return std::launder(_data + index);
    }

    template <typename T, typename Allocator>
    T const* Vector<T, Allocator>::get_ptr(size_type index) const {
        return std::launder(_data + index);
    }

    template <typename T, typename Allocator>
    T* Vector<T, Allocator>::allocate(size_type const size) {
        void* mem = _allocator.allocate(size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
        return static_cast<T*>(mem);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::deallocate(void* mem, size_type const size) {
        _allocator.deallocate(mem, size * static_cast<isize>(sizeof(T)), static_cast<isize>(alignof(T)));
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::ensure_capacity(size_type requested_capacity) {
        if (requested_capacity > _capacity) {
            size_type new_capacity = _capacity;
            while (new_capacity < requested_capacity) {
                new_capacity *= 2;
            }

            T* new_data = allocate(new_capacity);
            try {
                // TODO: More reallocation options
                if constexpr (atl::is_move_constructible<T>) {
                    atl::uninitialized_move(_data, _data + _size, new_data);
                } else {
                    atl::uninitialized_copy(_data, _data + _size, new_data);
                }
            } catch (...) {
                deallocate(new_data, new_capacity);
                throw;
            }
            destruct_n(_data, _size);
            deallocate(_data, _capacity);
            _data = new_data;
            _capacity = new_capacity;
        }
    }
} // namespace anton_engine::atl

namespace anton_engine {
    template <typename T, typename Allocator>
    void serialize(serialization::Binary_Output_Archive& out, atl::Vector<T, Allocator> const& vec) {
        using size_type = typename atl::Vector<T, Allocator>::size_type;
        size_type capacity = vec.capacity(), size = vec.size();
        out.write(capacity);
        out.write(size);
        for (T const& elem: vec) {
            serialize(out, elem);
        }
    }

    template <typename T, typename Allocator>
    void deserialize(serialization::Binary_Input_Archive& in, atl::Vector<T, Allocator>& vec) {
        using size_type = typename atl::Vector<T, Allocator>::size_type;
        size_type capacity, size;
        in.read(capacity);
        in.read(size);
        vec.clear();
        vec.set_capacity(capacity);
        if constexpr (atl::is_default_constructible<T>) {
            vec.resize(size);
            try {
                for (T& elem: vec) {
                    deserialize(in, elem);
                }
            } catch (...) {
                // TODO: Move stream backwards to maintain weak guarantee
                atl::destruct_n(vec.data(), size);
                throw;
            }
        } else {
            size_type n = size;
            try {
                for (; n > 0; --n) {
                    Stack_Allocate<T> elem;
                    vec.push_back(atl::move(elem.reference()));
                    deserialize(in, vec.back());
                }
                vec._size = size;
            } catch (...) {
                // TODO: Move stream backwards to maintain weak guarantee
                atl::destruct_n(vec.data(), size - n);
                throw;
            }
        }
    }
} // namespace anton_engine
