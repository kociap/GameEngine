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
        return *reinterpret_cast<T*>(std::launder(&storage[index]));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::at_unchecked(size_type index) const {
        return *reinterpret_cast<T const*>(std::launder(&storage[index]));
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
        return at_unchecked(size - 1);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_reference Static_Vector<T, _capacity>::back() {
        return at_unchecked(size - 1);
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::pointer Static_Vector<T, _capacity>::data() {
        return reinterpret_cast<T*>(std::launder(storage));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_pointer Static_Vector<T, _capacity>::data() const {
        return reinterpret_cast<T const*>(std::launder(storage));
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::begin() {
        return iterator(storage);
    }
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::begin() const {
        return const_iterator(storage);
    }
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::cbegin() const {
        return const_iterator(storage);
    }
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::end() {
        return iterator(storage + _size);
    }
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::end() const {
        return const_iterator(storage + _size);
    }
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::const_iterator Static_Vector<T, _capacity>::cend() const {
        return const_iterator(storage + _size);
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
    bool Static_Vector<T, _capacity>::empty() const {
        return _size == 0;
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
    void Static_Vector<T, _capacity>::reserve(size_type new_cap) {}

    template <typename T, uint64_t _capacity>
    constexpr typename Static_Vector<T, _capacity>::size_type Static_Vector<T, _capacity>::capacity() const {
        return _capacity;
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::shrink_to_fit() {}

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::clear() {
        for (size_type i = 0; i < _size; ++i) {
            T& elem = at_unchecked(i);
            ~elem();
        }
    }

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, const value_type& value);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, size_type n, const value_type& value);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, value_type&& value);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, std::initializer_list<value_type> ilist);
    template <typename T, uint64_t _capacity>
    template <typename Iterator>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::insert(const_iterator position, Iterator first, Iterator last);

    template <typename T, uint64_t _capacity>
    template <typename... Args>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::emplace(const_iterator position, Args&&... args);

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(const_iterator position);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(const_iterator first, const_iterator last);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase_unsorted(const_iterator position);
    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase(size_type index);

    template <typename T, uint64_t _capacity>
    typename Static_Vector<T, _capacity>::iterator Static_Vector<T, _capacity>::erase_unsorted(size_type index) {
        T* elem = storage + index;
        T* last_elem = storage + _size - 1;
        static_assert(std::is_nothrow_move_constructible_v<T>, "T is not nothrow move constructible"); // TODO temporarily
        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            elem->~T();
            if (elem != last_elem) {
                attempt_move(last_elem, elem);
                last_elem->~T();
            }
        }
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::push_back(T const& val) {
        check_size();
        new (storage + _size) T(val);
        ++size;
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::push_back(T&& val) {
        check_size();
        new (storage + _size) T(std::move(val));
        ++size;
    }

    template <typename T, uint64_t _capacity>
    template <typename... Args>
    typename Static_Vector<T, _capacity>::reference Static_Vector<T, _capacity>::emplace_back(Args&&... args) {
        check_size();
        new (storage + _size) T(std::forward<Args>(args)...);
        return at_unchecked(_size++);
    }

    template <typename T, uint64_t _capacity>
    void Static_Vector<T, _capacity>::pop_back() {
        T& last_elem = back();
        ~last_elem();
        --_size;
    }
} // namespace containers