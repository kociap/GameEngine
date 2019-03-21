namespace containers {
    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector() : storage(nullptr, Deleter(allocator, _capacity)) {
        storage.reset(allocator.allocate(_capacity));
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type cap) : _capacity(cap), storage(nullptr, Deleter(allocator, cap)) {
        storage.reset(allocator.allocate(cap));
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector const& original) : storage(nullptr, Deleter(allocator, _capacity)) {
        throw std::runtime_error("Not implemented");
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector&& from) noexcept
        : storage(std::move(from.storage)), allocator(std::move(from.allocator)), _capacity(std::move(from._capacity)), _size(std::move(from._size)) {}

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector const& original) {
        throw std::runtime_error("Not implemented");
        return *this;
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& from) noexcept {
        storage = std::move(from.storage);
        storage = std::move(from.storage);
        allocator = std::move(from.allocator);
        _capacity = std::move(from._capacity);
        _size = std::move(from._size);
        return *this;
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::~Vector() {}

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::clear() {
        for (size_type i = 0; i < _size; ++i) {
            T* elem_ptr = storage.get() + i;
            elem_ptr->~T();
        }
        _size = 0;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::size() const {
        return _size;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::capacity() const {
        return _capacity;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::pointer Vector<T, Allocator>::data() {
        return storage_ptr.get();
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_pointer Vector<T, Allocator>::data() const {
        return storage_ptr.get();
    }

    template <typename T, typename Allocator>
    template <typename... CtorArgs>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::emplace_back(CtorArgs&&... args) {
        check_size();
        new (storage.get() + _size) T(std::forward<CtorArgs>(args)...);
        T& elem = *(storage.get() + _size);
        ++_size;
        return elem;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted(size_type index) {
        if (index >= _size) {
            throw std::invalid_argument("index out of range");
        }

        erase_unsorted_unchecked(index);
    }

    // TODO copy constructible
    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted_unchecked(size_type index) {
        T* array_ptr = storage.get();
        T* elem_ptr = array_ptr + index;
        T* last_elem_ptr = array_ptr + _size - 1;
        // if constexpr (std::is_nothrow_move_constructible_v<T>) {
        static_assert(std::is_nothrow_move_constructible_v<T>, "Type is not nothrow move constructible");
        elem_ptr->~T();
        if (elem_ptr != last_elem_ptr) {
            attempt_move(last_elem_ptr, elem_ptr);
            last_elem_ptr->~T();
        }
        // } else {
        //     static_assert(std::is_copy_constructible_v<T>, "Type is neither nothrow move constructible nor copy constructible");

        //     T copy(*elem_ptr);
        //     try {
        //         elem_ptr->~T();
        //         attempt_construct(last_elem_ptr, elem_ptr);
        //     } catch {  // This catch-all is bad
        //                // TODO Handle errors (maybe)
        //         throw; // temporarily rethrow
        //     }
        // }
        --_size;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::operator[](size_type index) {
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::operator[](size_type index) const {
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::at(size_type index) {
        if (index >= _size) {
            throw std::invalid_argument("index out of range");
        }
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at(size_type index) const {
        if (index >= _size) {
            throw std::invalid_argument("index out of range");
        }
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::at_unchecked(size_type index) {
        T* object = storage.get() + index;
        return *object;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at_unchecked(size_type index) const {
        T* object = storage.get() + index;
        return *object;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() {
        return iterator(storage.get());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() {
        return iterator(storage.get() + _size);
    }
    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const {
        return const_iterator(storage.get());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const {
        return const_iterator(storage.get() + _size);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cbegin() const {
        return const_iterator(storage.get());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cend() const {
        return const_iterator(storage.get() + _size);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rbegin() {
        return reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rend() {
        return reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crbegin() const {
        return const_reverse_iterator(cend());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crend() const {
        return const_reverse_iterator(cbegin());
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_construct(T* from, T* to) {
        new (to) T(*from);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_move(T* from, T* to) {
        new (to) T(std::move(*from));
    }

    // Reallocates current storage
    // Uses move constructor if T is nothrow move constructible
    // Otherwise copies all elements
    template <typename T, typename Allocator>
    void Vector<T, Allocator>::reallocate(storage_ptr const& from, storage_ptr const& to) {
        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            for (size_type i = 0; i < _size; ++i) {
                attempt_move(from.get() + i, to.get() + i);
            }
        } else {
            for (size_type i = 0; i < _size; ++i) {
                try {
                    attempt_construct(from.get() + i, to.get() + i);
                } catch (...) { // This catch-all is bad
                                // TODO Handle errors (maybe)
                    throw;      // temporarily rethrow
                }
            }
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::check_size() {
        if (_size == _capacity) {
            size_type new__capacity = _capacity * 2;
            storage_ptr new_storage(nullptr, Deleter(allocator, new__capacity));
            new_storage.reset(allocator.allocate(new__capacity));
            reallocate(storage, new_storage);
            std::swap(storage, new_storage);
            _capacity = new__capacity;
        }
    }
} // namespace containers