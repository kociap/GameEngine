namespace anton_engine::atl {
    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(): _size(0) {}

    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s): _size(s) {
        if (s > Capacity) {
            throw Invalid_Argument_Exception("Size is greater than capacity.");
        }

        uninitialized_default_construct_n(get_ptr(0), _size);
    }

    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(size_type const s, value_type const& v): _size(s) {
        if (s > Capacity) {
            throw Invalid_Argument_Exception("Size is greater than capacity.");
        }

        uninitialized_fill_n(get_ptr(0), _size, v);
    }

    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array const& other): _size(other._size) {
        uninitialized_copy_n(other.get_ptr(0), _size, get_ptr(0));
    }

    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::Fixed_Array(Fixed_Array&& other) noexcept: _size(other._size) {
        uninitialized_move_n(other.get_ptr(0), _size, get_ptr(0));
    }

    template <typename T, i64 Capacity>
    template<typename Input_Iterator>
    Fixed_Array<T, Capacity>::Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last) {
        i64 const distance = last - first;
        if(distance > Capacity) {
            throw Invalid_Argument_Exception("Distance between last and first is greater than the capacity.");
        }

        uninitialized_copy(first, last, get_ptr(0));
    }

    template <typename T, i64 Capacity>
    template <typename... Args>
    Fixed_Array<T, Capacity>::Fixed_Array(Variadic_Construct_Tag, Args&&... args): _size(sizeof...(Args)) {
        if (sizeof...(Args) > Capacity) {
            throw Invalid_Argument_Exception("Size is greater than capacity.");
        }

        uninitialized_variadic_construct(get_ptr(0), forward<Args>(args)...);
    }

    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>::~Fixed_Array() {
        destruct_n(get_ptr(0), _size);
    }

    // TODO: exception guarantees?
    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array const& other) {
        if (_size >= other._size) {
            copy(other.get_ptr(0), other.get_ptr(other._size), get_ptr(0));
            destruct_n(get_ptr(other._size), _size - other._size);
            _size = other._size;
        } else {
            copy(other.get_ptr(0), other.get_ptr(_size), get_ptr(0));
            uninitialized_copy(other.get_ptr(_size), other.get_ptr(other._size), get_ptr(_size));
            _size = other._size;
        }
        return *this;
    }

    // TODO: exception guarantees?
    template <typename T, i64 Capacity>
    Fixed_Array<T, Capacity>& Fixed_Array<T, Capacity>::operator=(Fixed_Array&& from) noexcept {
        if (_size >= from._size) {
            move(from.get_ptr(0), from.get_ptr(from._size), get_ptr(0));
            destruct_n(get_ptr(from._size), _size - from._size);
            _size = from._size;
        } else {
            move(from.get_ptr(0), from.get_ptr(_size), get_ptr(0));
            uninitialized_move(from.get_ptr(_size), from.get_ptr(from._size), get_ptr(_size));
            _size = from._size;
        }
        return *this;
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) -> reference {
        return *get_ptr(index);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::operator[](size_type index) const -> const_reference {
        return *get_ptr(index);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() -> pointer {
        return get_ptr(0);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::data() const -> const_pointer {
        return get_ptr(0);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() -> iterator {
        return get_ptr(0);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() -> iterator {
        return get_ptr(_size);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::begin() const -> const_iterator {
        return get_ptr(0);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::end() const -> const_iterator {
        return get_ptr(_size);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cbegin() const -> const_iterator {
        return get_ptr(0);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::cend() const -> const_iterator {
        return get_ptr(_size);
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::size() const -> size_type {
        return _size;
    }

    template <typename T, i64 Capacity>
    auto Fixed_Array<T, Capacity>::capacity() const -> size_type {
        return Capacity;
    }

    template <typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::resize(size_type const s) {
        ANTON_VERIFY(s <= Capacity && s >= 0, "Requested size was outside the range [0, capacity()].");
        if (s >= _size) {
            uninitialized_default_construct_n(get_ptr(_size), s - _size);
        } else {
            destruct_n(get_ptr(s), _size - s);
        }
        _size = s;
    }

    template <typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::resize(size_type const s, T const& v) {
        ANTON_VERIFY(s <= Capacity && s >= 0, "Requested size was outside the range [0, capacity()].");
        if (s >= _size) {
            uninitialized_fill_n(get_ptr(_size), s - _size);
        } else {
            destruct_n(get_ptr(s), _size - s);
        }
        _size = s;
    }

    template <typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::clear() {
        destruct_n(get_ptr(0), _size);
        _size = 0;
    }

    template <typename T, i64 Capacity>
    template <typename... Args>
    auto Fixed_Array<T, Capacity>::emplace_back(Args&&... args) -> reference {
        ANTON_VERIFY(_size < Capacity, "Cannot emplace_back element in a full Fixed_Array.");
        T* const elem = get_ptr(_size);
        construct(elem, forward<Args>(args)...);
        _size += 1;
        return *elem;
    }

    template <typename T, i64 Capacity>
    void Fixed_Array<T, Capacity>::pop_back() {
        ANTON_VERIFY(_size > 0, "Trying to pop an element from an empty Fixed_Array");
        T* const elem = get_ptr(_size - 1);
        destruct(elem);
        _size -= 1;
    }

    template <typename T, i64 Capacity>
    T* Fixed_Array<T, Capacity>::get_ptr(size_type const index) {
        return std::launder(reinterpret_cast<T*>(_data + index));
    }

    template <typename T, i64 Capacity>
    T const* Fixed_Array<T, Capacity>::get_ptr(size_type const index) const {
        return std::launder(reinterpret_cast<T const*>(_data + index));
    }

    template<typename T, i64 Capacity>
    template <typename... Args>
    void Fixed_Array<T, Capacity>::construct(void* ptr, Args&&... args) {
        if constexpr (is_constructible<T, Args&&...>) {
            ::new (ptr) T(forward<Args>(args)...);
        } else {
            ::new (ptr) T{forward<Args>(args)...};
        }
    }
} // namespace anton_engine::atl
