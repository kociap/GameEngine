#ifndef CORE_ATL_MEMORY_HPP_INCLUDE
#define CORE_ATL_MEMORY_HPP_INCLUDE

#include <core/atl/detail/memory_common.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/type_traits.hpp>

#include <string.h> // memcpy, memmove

// TODO alloc_aligned

namespace anton_engine::atl {
    template <typename T>
    void destruct(T* elem) {
        elem->~T();
    }

    template <typename Forward_Iterator>
    void destruct(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr (!atl::is_trivially_destructible<value_type>) {
            for (; first != last; ++first) {
                first->~value_type();
            }
        }
    }

    template <typename Forward_Iterator, typename Count>
    void destruct_n(Forward_Iterator first, Count n) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr (!atl::is_trivially_destructible<value_type>) {
            for (; n > 0; --n, ++first) {
                first->~value_type();
            }
        }
    }

    template <typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; first != last; ++first, ++dest) {
                ::new (atl::addressof(*dest)) value_type(*first);
            }
        } catch (...) {
            atl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; n > 0; --n, ++first, ++dest) {
                ::new (atl::addressof(*dest)) value_type(*first);
            }
        } catch (...) {
            atl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_move(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; first != last; ++first, ++dest) {
                ::new (atl::addressof(*dest)) value_type(atl::move(*first));
            }
        } catch (...) {
            atl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_move_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator dest_copy = dest;
        try {
            for (; n > 0; --n, ++first, ++dest) {
                ::new (atl::addressof(*dest)) value_type(atl::move(*first));
            }
        } catch (...) {
            atl::destruct(dest_copy, dest);
            throw;
        }
        return dest;
    }

    template <typename Forward_Iterator>
    void uninitialized_default_construct(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; first != last; ++first) {
                ::new (atl::addressof(*first)) value_type();
            }
        } catch (...) {
            atl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename Count>
    void uninitialized_default_construct_n(Forward_Iterator first, Count n) {
        // TODO: Trivially constructible
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; n > 0; --n, ++first) {
                ::new (atl::addressof(*first)) value_type();
            }
        } catch (...) {
            atl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename T>
    void uninitialized_fill(Forward_Iterator first, Forward_Iterator last, T const& val) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; first != last; ++first) {
                ::new (atl::addressof(*first)) value_type(val);
            }
        } catch (...) {
            atl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename Count, typename T>
    void uninitialized_fill_n(Forward_Iterator first, Count n, T const& val) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            for (; n > 0; --n, ++first) {
                ::new (atl::addressof(*first)) value_type(val);
            }
        } catch (...) {
            atl::destruct(first_copy, first);
            throw;
        }
    }

    template <typename Forward_Iterator, typename... Ts>
    void uninitialized_variadic_construct(Forward_Iterator first, Ts&&... vals) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        Forward_Iterator first_copy = first;
        try {
            (..., ::new (atl::addressof(*(first++))) value_type(atl::forward<Ts>(vals)));
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
        using input_iterator_cat = typename atl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                   atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for (; first != last; ++first, ++dest) {
                *dest = *first;
            }
            return dest;
        }
    }

    // copy_backward
    //
    // Copy assigns elements from range [first, last[ to dest in reverse order (last element is copied first) decrementing dest after each copy.
    // The back of the range [first, last[ may overlap the front of the range [dest - (first - last), dest[.
    //
    // Returns iterator to the beginning of the destination range.
    //
    // Example usage:
    //   atl::copy_backward(array.begin(), array.end(), dest_array.end());
    //
    // Reference implementation:
    //   template<typename Bidirectional_Iterator>
    //   Bidirectional_Iterator copy_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Bidirectional_Iterator dest) {
    //       while (last != first) {
    //           *--dest = *--last;
    //       }
    //       return dest;
    //   }
    //
    template <typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator copy_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                   atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while (last != first) {
                *--dest = *--last;
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
        using input_iterator_cat = typename atl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous = atl::conjunction<atl::Is_Same<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                                                              atl::Is_Same<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for (; first != last; ++first, ++dest) {
                *dest = atl::move(*first);
            }
            return dest;
        }
    }

    // move_backward
    //
    // Move assigns elements from range [first, last[ to dest in reverse order (last element is copied first) decrementing dest after each move.
    // The back of the range [first, last[ may overlap the front of the range [dest - (first - last), dest[.
    //
    // Returns iterator to the beginning of the destination range.
    //
    // Example usage:
    //   atl::move_backward(array.begin(), array.end(), dest_array.end());
    //
    // Reference implementation:
    //   template<typename Bidirectional_Iterator>
    //   Bidirectional_Iterator move_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Bidirectional_Iterator dest) {
    //       while (last != first) {
    //           *--dest = atl::move(*--last);
    //       }
    //       return dest;
    //   }
    //
    template <typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator move_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                   atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr (both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while (last != first) {
                *--dest = *--last;
            }
            return dest;
        }
    }
} // namespace anton_engine::atl
#endif // !CORE_ATL_MEMORY_HPP_INCLUDE
