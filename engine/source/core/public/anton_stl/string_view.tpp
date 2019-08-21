namespace anton_stl {

    template <typename T>
    inline constexpr Basic_String_View<T>::Basic_String_View(): storage(nullptr), _size(0) {}

    template <typename T>
    inline constexpr Basic_String_View<T>::Basic_String_View(T const* s, size_type n): storage(s), _size(n) {}

    template <typename T>
    inline constexpr Basic_String_View<T>::Basic_String_View(T const* s): storage(s), _size(0) {
        if (storage != nullptr) {
            T const* s_copy = storage;
            while (*s_copy)
                ++s_copy;
            _size = static_cast<size_type>(s_copy - storage);
        }
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::operator[](size_type index) const -> const_reference {
        return storage[index];
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::begin() const -> const_iterator {
        return storage;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::cbegin() const -> const_iterator {
        return storage;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::end() const -> const_iterator {
        return storage + _size;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::cend() const -> const_iterator {
        return storage + _size;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::rbegin() const -> const_reverse_iterator {
        return reverse_iterator(end());
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::crbegin() const -> const_reverse_iterator {
        return reverse_iterator(end());
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::rend() const -> const_reverse_iterator {
        return reverse_iterator(begin());
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::crend() const -> const_reverse_iterator {
        return reverse_iterator(begin());
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::size() const -> size_type {
        return _size;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::max_size() const -> size_type {
        return (~size_type(0)) >> 1;
    }

    template <typename T>
    inline constexpr auto Basic_String_View<T>::data() const -> const_pointer {
        return storage;
    }
} // namespace anton_stl
