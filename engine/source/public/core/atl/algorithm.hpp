#ifndef CORE_ATL_ALGORITHM_HPP_INCLUDE
#define CORE_ATL_ALGORITHM_HPP_INCLUDE

#include <core/exception.hpp>

namespace anton_engine::atl {
    // fill_with_consecutive
    // Fills range [first, last[ with consecutive values starting at value.
    //
    template<typename Forward_Iterator, typename T>
    void fill_with_consecutive(Forward_Iterator first, Forward_Iterator last, T value) {
        for(; first != last; ++first, ++value) {
            *first = value;
        }
    }

    // find
    // Linearily searches the range [first, last[.
    //
    // Returns: The iterator to the first element in the range [first, last[ that satisfies
    // the condition *iterator == value or last if no such iterator is found.
    //
    // Complexity: At most 'last - first' comparisons.
    //
    template<typename Input_Iterator, typename T>
    [[nodiscard]] inline Input_Iterator find(Input_Iterator first, Input_Iterator last, T const& value) {
        while(first != last && *first != value) {
            ++first;
        }
        return first;
    }

    // find_if
    // Linearily searches the range [first, last[.
    //
    // Returns: The iterator to the first element in the range [first, last[ that satisfies
    // the condition predicate(*iterator) == true or last if no such iterator is found.
    //
    // Complexity: At most 'last - first' applications of the predicate.
    //
    template<typename Input_Iterator, typename Predicate>
    [[nodiscard]] inline Input_Iterator find_if(Input_Iterator first, Input_Iterator last, Predicate predicate) {
        while(first != last && !predicate(*first)) {
            ++first;
        }
        return first;
    }

    template<typename Input_Iterator, typename Predicate>
    [[nodiscard]] inline bool any_of(Input_Iterator first, Input_Iterator last, Predicate predicate) {
        for(; first != last; ++first) {
            if(predicate(*first)) {
                return true;
            }
        }

        return false;
    }

    // unique
    // Eliminates all except the first element from every consecutive group of equivalent elements from
    // the range [first, last[ and returns a past-the-end iterator for the new logical end of the range.
    //
    template<typename Forward_Iterator, typename Predicate>
    Forward_Iterator unique(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
        if(first == last) {
            return last;
        }

        // Find first group that consists of > 2 duplicates.
        Forward_Iterator next = first;
        ++next;
        while(next != last && !predicate(*first, *next)) {
            ++first;
            ++next;
        }

        if(next != last) {
            for(; next != last; ++next) {
                if(!predicate(*first, *next)) {
                    ++first;
                    *first = atl::move(*next);
                }
            }
            return ++first;
        } else {
            return last;
        }
    }

    // unique
    // Eliminates all except the first element from every consecutive group of equivalent elements from
    // the range [first, last[ and returns a past-the-end iterator for the new logical end of the range.
    //
    template<typename Forward_Iterator>
    Forward_Iterator unique(Forward_Iterator begin, Forward_Iterator end) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        return unique(begin, end, [](value_type const& lhs, value_type const& rhs) { return lhs == rhs; });
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_ALGORITHM_HPP_INCLUDE
