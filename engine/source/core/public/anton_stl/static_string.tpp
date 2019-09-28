namespace anton_engine::anton_stl {
    template <typename T, anton_stl::ssize_t Capacity>
    inline Basic_Static_String<T, Capacity>::operator anton_stl::Basic_String_View<T>() const {
        return Basic_String_View<T>(data(), size());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::operator[](size_type index) -> reference {
        return *get_ptr()[index];
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::operator[](size_type index) const -> const_reference {
        return *get_ptr()[index];
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::begin() -> iterator {
        return get_ptr();
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::begin() const -> const_iterator {
        return get_ptr();
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::cbegin() const -> const_iterator {
        return get_ptr();
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::end() -> iterator {
        return get_ptr() + _size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::end() const -> const_iterator {
        return get_ptr() + _size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::cend() const -> const_iterator {
        return get_ptr() + _size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::rbegin() -> reverse_iterator {
        return reverse_iterator(end());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::rbegin() const -> reverse_const_iterator {
        return reverse_iterator(end());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::crbegin() const -> reverse_const_iterator {
        return reverse_iterator(end());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::rend() -> reverse_iterator {
        return reverse_iterator(begin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::rend() const -> reverse_const_iterator {
        return reverse_const_iterator(begin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::crend() const -> reverse_const_iterator {
        return reverse_const_iterator(begin());
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::size() const -> size_type {
        return _size;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline constexpr auto Basic_Static_String<T, Capacity>::max_size() const -> size_type {
        return (~size_type(0)) >> 1;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline constexpr auto Basic_Static_String<T, Capacity>::capacity() const -> size_type {
        return Capacity;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Basic_Static_String<T, Capacity>::resize(size_type n, value_type v) {
        if (n > Capacity) {
            throw std::length_error("Attempt to construct more than capacity() elements in Basic_Static_String");
        }

        if (n > _size) { // Upsizing probably is the more likely case
            for (T* ptr = get_ptr() + _size; _size != n; ++ptr, ++_size) {
                *ptr = v;
            }
        } else {
            for (T* ptr = get_ptr() + _size; _size != n; --ptr, --_size) {
                *ptr = value_type(0);
            }
        }
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void Basic_Static_String<T, Capacity>::force_size(size_type n) {
        if (n > Capacity) {
            throw std::length_error("Attempt to force size that is greater than capacity() in Basic_Static_String");
        }

        _size = n;
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::data() -> pointer {
        return get_ptr();
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline auto Basic_Static_String<T, Capacity>::data() const -> const_pointer {
        return get_ptr();
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline T* Basic_Static_String<T, Capacity>::get_ptr() {
        return std::launder(reinterpret_cast<T*>(storage));
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline T const* Basic_Static_String<T, Capacity>::get_ptr() const {
        return std::launder(reinterpret_cast<T const*>(storage));
    }
} // namespace anton_engine::anton_stl

namespace anton_engine {
    template <typename T, anton_stl::ssize_t Capacity>
    inline void serialize(serialization::Binary_Output_Archive& out, anton_stl::Basic_Static_String<T, Capacity> const& str) {
        out.write(str.size());
        auto sizeof_T = static_cast<typename anton_stl::Basic_Static_String<T, Capacity>::size_type>(sizeof(T));
        out.write_binary(str.data(), str.size() * sizeof_T);
    }

    template <typename T, anton_stl::ssize_t Capacity>
    inline void deserialize(serialization::Binary_Input_Archive& in, anton_stl::Basic_Static_String<T, Capacity>& str) {
        typename anton_stl::Basic_Static_String<T, Capacity>::size_type size;
        in.read(size);
        str.force_size(size);
        auto sizeof_T = static_cast<typename anton_stl::Basic_Static_String<T, Capacity>::size_type>(sizeof(T));
        in.read_binary(str.data(), str.size() * sizeof_T);
    }
} // namespace anton_engine
