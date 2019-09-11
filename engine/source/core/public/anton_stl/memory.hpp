#ifndef CORE_ANTON_STL_MEMORY_HPP_INCLUDE
#define CORE_ANTON_STL_MEMORY_HPP_INCLUDE

#include <anton_stl/detail/memory_common.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/type_traits.hpp>

#include <string.h> // memcpy, memmove

// TODO alloc_aligned

namespace anton_engine::anton_stl {
    template <typename T>
    void destruct(T* elem) {
        elem->~T();
    }

    template <typename Forward_Iterator>
    void destruct(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr (!anton_stl::is_trivially_destructible<value_type>) {
            for (; first != last; ++first) {
                first->~value_type();
            }
        }
    }

    template <typename Forward_Iterator, typename Count>
    void destruct_n(Forward_Iterator first, Count n) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr (!anton_stl::is_trivially_destructible<value_type>) {
            for (; n > 0; --n, ++first) {
                first->~value_type();
            }
        }
    }

    template <typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; first != last; ++first, ++dest) {
                ::new (anton_stl::addressof(*dest)) value_type(*first);
            }
        } catch (...) {
            anton_stl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; n > 0; --n, ++first, ++dest) {
                ::new (anton_stl::addressof(*dest)) value_type(*first);
            }
        } catch (...) {
            anton_stl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_move(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; first != last; ++first, ++dest) {
                ::new (anton_stl::addressof(*dest)) value_type(anton_stl::move(*first));
            }
        } catch (...) {
            anton_stl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_move_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; n > 0; --n, ++first, ++dest) {
                ::new (anton_stl::addressof(*dest)) value_type(anton_stl::move(*first));
            }
        } catch (...) {
            anton_stl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Forward_Iterator>
    void uninitialized_default_construct(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; first != last; ++first) {
                ::new (anton_stl::addressof(*first)) value_type();
            }
        } catch (...) {
            anton_stl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename Count>
    void uninitialized_default_construct_n(Forward_Iterator first, Count n) {
        // TODO: Trivially constructible
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; n > 0; --n, ++first) {
                ::new (anton_stl::addressof(*first)) value_type();
            }
        } catch (...) {
            anton_stl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename T>
    void uninitialized_fill(Forward_Iterator first, Forward_Iterator last, T const& val) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; first != last; ++first) {
                ::new (anton_stl::addressof(*first)) value_type(val);
            }
        } catch (...) {
            anton_stl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename Count, typename T>
    void uninitialized_fill_n(Forward_Iterator first, Count n, T const& val) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; n > 0; --n, ++first) {
                ::new (anton_stl::addressof(*first)) value_type(val);
            }
        } catch (...) {
            anton_stl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename... Ts>
    void uninitialized_variadic_construct(Forward_Iterator first, Ts&&... vals) {
        using value_type = typename anton_stl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            (..., ::new (anton_stl::addressof(*(first++))) value_type(anton_stl::forward<Ts>(vals)));
        } catch (...) {
            destruct(first_copy, first);
            throw;
        }
    }

    // copy
    //
    // Returns: An iterator to the end of the dest range.
    //
    template <typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator copy(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename anton_stl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename anton_stl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename anton_stl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename anton_stl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            anton_stl::conjunction<anton_stl::Is_Iterator_Category<input_iterator_cat, anton_stl::Contiguous_Iterator_Tag>,
                                   anton_stl::Is_Iterator_Category<output_iterator_cat, anton_stl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && anton_stl::is_trivial<input_value_type> && anton_stl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            return static_cast<Output_Iterator>(memmove(dest, first, (last - first) * sizeof(input_value_type)));
        } else {
            for (; first != last; ++first, ++dest) {
                *dest = *first;
            }
            return dest;
        }
    }

    // move
    // Moves elements from [first, last[ to dest.
    //
    // Returns: An iterator to the end of the dest range.
    //
    template <typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator move(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename anton_stl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename anton_stl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename anton_stl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename anton_stl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous = anton_stl::conjunction<anton_stl::Is_Same<input_iterator_cat, anton_stl::Contiguous_Iterator_Tag>,
                                                                              anton_stl::Is_Same<output_iterator_cat, anton_stl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && anton_stl::is_trivial<input_value_type> && anton_stl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            return static_cast<Output_Iterator>(memmove(dest, first, (last - first) * sizeof(input_value_type)));
        } else {
            for (; first != last; ++first, ++dest) {
                *dest = anton_stl::move(*first);
            }
            return dest;
        }
    }
} // namespace anton_engine::anton_stl
#endif // !CORE_ANTON_STL_MEMORY_HPP_INCLUDE
