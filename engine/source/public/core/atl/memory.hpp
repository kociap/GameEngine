#ifndef CORE_ATL_MEMORY_HPP_INCLUDE
#define CORE_ATL_MEMORY_HPP_INCLUDE

#include <core/anton_crt.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/type_traits.hpp>

// TODO alloc_aligned
// TODO: add constexpr if for trivial operations.

namespace anton_engine::atl {
    // addressof
    template<typename T>
    [[nodiscard]] constexpr T* addressof(T& value) {
#ifndef ANTON_HAS_NO_BUILTIN_ADDRESSOF
        return __builtin_addressof(value);
#else
        // Note: We do not support volatile.
        return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<char const&>(value)));
#endif
    }

    // Issue 2598 "addressof works on temporaries"
    // Deleted to prevent taking the address of temporaries.
    template<typename T>
    T const* addressof(T const&&) = delete;

    template<class T>
    [[nodiscard]] constexpr T* launder(T* ptr) noexcept {
        static_assert(!is_function<T> && !is_void<T>, "N4727 21.6.4 [ptr.launder]/3: The program is ill-formed if T is a function type or void const.");
        return __builtin_launder(ptr);
    }

    template<typename T, typename... Args>
    void construct(T* pointer, Args&&... args) {
        if constexpr(atl::is_constructible<T, decltype(atl::forward<Args>(args))...>) {
            ::new((void*)pointer) T(atl::forward<Args>(args)...);
        } else {
            ::new((void*)pointer) T{atl::forward<Args>(args)...};
        }
    }

    template<typename T>
    void destruct(T* pointer) {
        pointer->~T();
    }

    template<typename Forward_Iterator>
    void destruct([[maybe_unused]] Forward_Iterator first, [[maybe_unused]] Forward_Iterator last) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!atl::is_trivially_destructible<value_type>) {
            for(; first != last; ++first) {
                first->~value_type();
            }
        }
    }

    template<typename Forward_Iterator, typename Count>
    void destruct_n([[maybe_unused]] Forward_Iterator first, [[maybe_unused]] Count n) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!atl::is_trivially_destructible<value_type>) {
            for(; n > 0; --n, ++first) {
                first->~value_type();
            }
        }
    }

    template<typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        for(; first != last; ++first, ++dest) {
            construct(atl::addressof(*dest), *first);
        }
        return dest;
    }

    template<typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_copy_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        for(; n > 0; --n, ++first, ++dest) {
            construct(atl::addressof(*dest), *first);
        }
        return dest;
    }

    template<typename Input_Iterator, typename Forward_Iterator>
    Forward_Iterator uninitialized_move(Input_Iterator first, Input_Iterator last, Forward_Iterator dest) {
        for(; first != last; ++first, ++dest) {
            construct(atl::addressof(*dest), atl::move(*first));
        }
        return dest;
    }

    template<typename Input_Iterator, typename Count, typename Forward_Iterator>
    Forward_Iterator uninitialized_move_n(Input_Iterator first, Count n, Forward_Iterator dest) {
        for(; n > 0; --n, ++first, ++dest) {
            construct(atl::addressof(*dest), atl::move(*first));
        }
        return dest;
    }

    template<typename Forward_Iterator>
    void uninitialized_default_construct([[maybe_unused]] Forward_Iterator first, [[maybe_unused]] Forward_Iterator last) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!atl::is_trivially_constructible<value_type>) {
            for(; first != last; ++first) {
                construct(atl::addressof(*first));
            }
        }
    }

    template<typename Forward_Iterator, typename Count>
    void uninitialized_default_construct_n(Forward_Iterator first, Count n) {
        using value_type = typename atl::Iterator_Traits<Forward_Iterator>::value_type;
        if constexpr(!atl::is_trivially_constructible<value_type>) {
            for(; n > 0; --n, ++first) {
                construct(atl::addressof(*first));
            }
        }
    }

    template<typename Forward_Iterator, typename T>
    void uninitialized_fill(Forward_Iterator first, Forward_Iterator last, T const& val) {
        for(; first != last; ++first) {
            construct(atl::addressof(*first), val);
        }
    }

    template<typename Forward_Iterator, typename Count, typename T>
    void uninitialized_fill_n(Forward_Iterator first, Count n, T const& val) {
        for(; n > 0; --n, ++first) {
            construct(atl::addressof(*first), val);
        }
    }

    template<typename Forward_Iterator, typename... Ts>
    void uninitialized_variadic_construct(Forward_Iterator first, Ts&&... vals) {
        (..., construct(atl::addressof(*(first++)), atl::forward<Ts>(vals)));
    }

    // copy
    // Copy assigns elements from [first, last[ to dest.
    //
    // Returns: An iterator to the end of the dest range.
    //
    template<typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator copy(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous = atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                                                        atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for(; first != last; ++first, ++dest) {
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
    // Returns:
    // An iterator to the beginning of the destination range.
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
    template<typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator copy_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous = atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                                                        atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while(last != first) {
                *--dest = *--last;
            }
            return dest;
        }
    }

    // move
    // Move assigns elements from [first, last[ to dest.
    //
    // Returns:
    // An iterator to the end of the dest range.
    //
    template<typename Input_Iterator, typename Output_Iterator>
    inline Output_Iterator move(Input_Iterator first, Input_Iterator last, Output_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Input_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Output_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Input_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Output_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous =
            atl::conjunction<atl::Is_Same<input_iterator_cat, atl::Contiguous_Iterator_Tag>, atl::Is_Same<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            memmove(dest, first, (usize)(last - first) * sizeof(input_value_type));
            return dest + (last - first);
        } else {
            for(; first != last; ++first, ++dest) {
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
    // Returns:
    // An iterator to the beginning of the destination range.
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
    template<typename Bidirectional_Iterator, typename Dest_Bidirectional_Iterator>
    inline Dest_Bidirectional_Iterator move_backward(Bidirectional_Iterator first, Bidirectional_Iterator last, Dest_Bidirectional_Iterator dest) {
        // TODO: Iterator unwrapping
        using input_iterator_cat = typename atl::Iterator_Traits<Bidirectional_Iterator>::iterator_category;
        using output_iterator_cat = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::iterator_category;
        using input_value_type = typename atl::Iterator_Traits<Bidirectional_Iterator>::value_type;
        using output_value_type = typename atl::Iterator_Traits<Dest_Bidirectional_Iterator>::value_type;

        constexpr bool both_iterators_are_contiguous = atl::conjunction<atl::Is_Iterator_Category<input_iterator_cat, atl::Contiguous_Iterator_Tag>,
                                                                        atl::Is_Iterator_Category<output_iterator_cat, atl::Contiguous_Iterator_Tag>>;
        if constexpr(both_iterators_are_contiguous && atl::is_trivial<input_value_type> && atl::is_same<input_value_type, output_value_type>) {
            // TODO: Assumes that contiguous iterators are pointers.
            Dest_Bidirectional_Iterator const dest_begin = dest - (last - first);
            memmove(dest_begin, first, (usize)(last - first) * sizeof(input_value_type));
            return dest_begin;
        } else {
            while(last != first) {
                *--dest = atl::move(*--last);
            }
            return dest;
        }
    }
} // namespace anton_engine::atl
#endif // !CORE_ATL_MEMORY_HPP_INCLUDE
