namespace anton_engine::anton_stl {
    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::operator[](size_type index) -> reference {
        return *get_ptr(index);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::operator[](size_type index) const -> const_reference {
        return *get_ptr_const(index);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::front() -> reference {
        return *get_ptr(0);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::front() const -> const_reference {
        return *get_ptr_const(0);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::back() -> reference {
        return *get_ptr(_size - 1);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::back() const -> const_reference {
        return *get_ptr_const(_size - 1);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::data() -> pointer {
        return get_ptr(0);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::data() const -> const_pointer {
        return get_ptr_const(0);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::begin() -> iterator {
        return iterator(get_ptr(0));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::begin() const -> const_iterator {
        return const_iterator(get_ptr(0));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::cbegin() const -> const_iterator {
        return const_iterator(get_ptr(0));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::end() -> iterator {
        return iterator(get_ptr(_size));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::end() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::cend() const -> const_iterator {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::rbegin() -> reverse_iterator {
        return reverse_iterator(end());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::rbegin() const -> reverse_const_iterator {
        return reverse_const_iterator(end());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::crbegin() const -> reverse_const_iterator {
        return reverse_const_iterator(cend());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::rend() -> reverse_iterator {
        return reverse_iterator(begin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::rend() const -> reverse_const_iterator {
        return reverse_const_iterator(begin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::crend() const -> reverse_const_iterator {
        return reverse_const_iterator(cbegin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline constexpr auto Static_Vector<T, Capacity>::size() const -> size_type {
        return _size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline constexpr auto Static_Vector<T, Capacity>::max_size() const -> size_type {
        return Capacity;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline constexpr auto Static_Vector<T, Capacity>::capacity() const -> size_type {
        return Capacity;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::resize(size_type n, T const& value) {
        if (n > Capacity) {
            throw std::length_error("Attempt to construct more than capacity() elements in Static_Vector");
        }

        if (n > _size) { // Upsizing is the more likely case
            anton_stl::uninitialized_fill(get_ptr(_size), get_ptr(n), value);
            _size = n;
        } else {
            anton_stl::destruct(get_ptr(n), get_ptr(_size));
            _size = n;
        }
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::resize(size_type n) {
        if (n > Capacity) {
            throw std::length_error("Attempt to construct more than capacity() elements in Static_Vector");
        }

        if (n > _size) { // Upsizing is the more likely case
            anton_stl::uninitialized_default_construct(get_ptr(_size), get_ptr(n));
            _size = n;
        } else {
            anton_stl::destruct(get_ptr(n), get_ptr(_size));
            _size = n;
        }
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::clear() {
        anton_stl::destruct_n(get_ptr(), _size);
        _size = 0;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::insert(const_iterator /* position */, const value_type & /* value */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::insert(const_iterator /* position */, size_type /* n */, const value_type & /* value */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::insert(const_iterator /* position */, value_type && /* value */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::insert(const_iterator /* position */, std::initializer_list<value_type> /* ilist */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    template <typename Iterator>
    inline auto Static_Vector<T, Capacity>::insert(const_iterator /* position */, Iterator /* first*/, Iterator /* last */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    template <typename... Args>
    inline auto Static_Vector<T, Capacity>::emplace(const_iterator /* position */, Args&&... /* args */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase(const_iterator /* position */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase(const_iterator /* first */, const_iterator /* last */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase_unsorted(const_iterator /* position */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase(size_type /* index */) -> iterator {
        return begin(); // TODO implement
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase_unsorted(size_type index) -> iterator {
        if (index > _size) {
            throw std::out_of_range("Index out of range");
        }

        return erase_unsorted_unchecked(index);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Static_Vector<T, Capacity>::erase_unsorted_unchecked(size_type index) -> iterator {
        T* elem = get_ptr(index);
        T* last_elem = get_ptr(_size - 1);
        *elem = std::move(*last_elem);
        --_size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::push_back(T const& val) {
        check_size();
        attempt_construct(get_ptr(_size), val);
        ++_size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::push_back(T&& val) {
        check_size();
        attempt_construct(get_ptr(_size), std::move(val));
        ++_size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    template <typename... Args>
    inline auto Static_Vector<T, Capacity>::emplace_back(Args&&... args) -> reference {
        check_size();
        attempt_construct(get_ptr(_size), std::forward<Args>(args)...);
        return *get_ptr(_size++);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::pop_back() {
        anton_stl::destruct(get_ptr(_size - 1));
        --_size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline T* Static_Vector<T, Capacity>::get_ptr(size_type index) {
        return std::launder(reinterpret_cast<T*>(storage)) + index;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline T const* Static_Vector<T, Capacity>::get_ptr_const(size_type index) const {
        return std::launder(reinterpret_cast<T const*>(storage)) + index;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    template <typename... Args>
    inline void Static_Vector<T, Capacity>::attempt_construct(void* ptr, Args&&... args) {
        if constexpr (std::is_constructible_v<T, Args&&...>) {
            ::new (ptr) T(std::forward<Args>(args)...);
        } else {
            ::new (ptr) T{std::forward<Args>(args)...};
        }
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::attempt_move(T* from, T* to) {
        ::new (to) T(std::move(*from));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Static_Vector<T, Capacity>::check_size() {
        if (_size == Capacity) {
            throw std::length_error("Attempt to construct more than capacity() elements in Static_Vector");
        }
    }
} // namespace anton_engine::anton_stl

namespace anton_engine {
    template <typename T, anton_stl::ssize_t Capacity>
    inline void serialize(serialization::Binary_Output_Archive& out, anton_stl::Static_Vector<T, Capacity> const& vec) {
        out.write(vec.size());
        for (T const& elem: vec) {
            serialize(out, elem);
        }
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void deserialize(serialization::Binary_Input_Archive& in, anton_stl::Static_Vector<T, Capacity>& vec) {
        typename anton_stl::Static_Vector<T, Capacity>::size_type size;
        in.read(vec._size);
        vec.clear();
        if constexpr (std::is_default_constructible_v<T>) {
            vec.resize(size);
            try {
                for (T& elem: vec) {
                    deserialize(in, elem);
                }
            } catch (...) {
                // TODO move stream backward
                anton_stl::destruct_n(vec.get_ptr(), size);
                throw;
            }
        } else {
            auto n = size;
            try {
                for (; n > 0; --n) {
                    Stack_Allocate<T> elem;
                    vec.push_back(std::move(elem.reference()));
                    deserialize(in, vec.back());
                }
                vec._size = size;
            } catch (...) {
                // TODO move stream backward
                anton_stl::destruct_n(vec.get_ptr(), size - n);
                throw;
            }
        }
    }
} // namespace anton_engine
