#ifndef CORE_MEMORY_MEMORY_HPP_INCLUDE
#define CORE_MEMORY_MEMORY_HPP_INCLUDE

#include <type_traits>

template <typename T>
void destruct(T* elem) {
    elem->~T();
}

template <typename T>
void destruct(T* first, T* last) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        for (; first != last; ++first) {
            first->~T();
        }
    }
}

template <typename T, typename Count>
void destruct_n(T* first, Count n) {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        for (; n > 0; --n, ++first) {
            first->~T();
        }
    }
}

template <typename T>
T* uninitialized_copy(T* first, T* last, T* dest) {
    T* dest_copy = dest;
    try {
        for (; first != last; ++first, ++dest) {
            ::new (dest) T(*first);
        }
    } catch (...) {
        destruct(dest_copy, dest);
        throw;
    }
    return dest;
}

template <typename T, typename Count>
T* uninitialized_copy_n(T* first, Count n, T* dest) {
    T* dest_copy = dest;
    try {
        for (; n > 0; --n, ++first, ++dest) {
            ::new (dest) T(*first);
        }
    } catch (...) {
        destruct(dest_copy, dest);
        throw;
    }
    return dest;
}

template <typename T>
T* uninitialized_move(T* first, T* last, T* dest) {
    T* dest_copy = dest;
    try {
        for (; first != last; ++first, ++dest) {
            ::new (dest) T(std::move(*first));
        }
    } catch (...) {
        destruct(dest_copy, dest);
        throw;
    }
    return dest;
}

template <typename T, typename Count>
T* uninitialized_move_n(T* first, Count n, T* dest) {
    T* dest_copy = dest;
    try {
        for (; n > 0; --n, ++first, ++dest) {
            ::new (dest) T(std::move(*first));
        }
    } catch (...) {
        destruct(dest_copy, dest);
        throw;
    }
    return dest;
}

template <typename T>
void uninitialized_default_construct(T* first, T* last) {
    T* first_copy = first;
    try {
        for (; first != last; ++first) {
            ::new (first) T();
        }
    } catch (...) {
        destruct(first_copy, first);
        throw;
    }
}

template <typename T, typename Count>
void uninitialized_default_construct_n(T* first, Count n) {
    T* first_copy = first;
    try {
        for (; n > 0; --n, ++first) {
            ::new (first) T();
        }
    } catch (...) {
        destruct(first_copy, first);
        throw;
    }
}

template <typename T>
void uninitialized_fill(T* first, T* last, T const& val) {
    T* first_copy = first;
    try {
        for (; first != last; ++first) {
            ::new (first) T(val);
        }
    } catch (...) {
        destruct(first_copy, first);
        throw;
    }
}

template <typename T, typename Count>
void uninitialized_fill_n(T* first, Count n, T const& val) {
    T* first_copy = first;
    try {
        for (; n > 0; --n, ++first) {
            ::new (first) T(val);
        }
    } catch (...) {
        destruct(first_copy, first);
        throw;
    }
}

#endif // !CORE_MEMORY_MEMORY_HPP_INCLUDE
