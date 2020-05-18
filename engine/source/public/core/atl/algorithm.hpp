#ifndef CORE_ATL_ALGORITHM_HPP_INCLUDE
#define CORE_ATL_ALGORITHM_HPP_INCLUDE

#include <core/atl/iterators.hpp>

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
    Forward_Iterator unique(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        return unique(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs == rhs; });
    }

    template<typename Forward_Iterator, typename Predicate>
    Forward_Iterator is_sorted(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
        if(first != last) {
            Forward_Iterator i = first;
            while(++i != last) {
                if(predicate(*i, *first)) {
                    return i;
                } else {
                    first = i;
                }
            }
        }
        return first;
    }

    template<typename Forward_Iterator, typename Predicate>
    void bubble_sort(Forward_Iterator first, Forward_Iterator last, Predicate predicate) {
        for(; first != last;) {
            Forward_Iterator i = first;
            Forward_Iterator next = first;
            ++next;
            bool swapped = false;
            for(; next != last; ++next, ++i) {
                if(predicate(*next, *i)) {
                    swap(*next, *i);
                    swapped = true;
                }
            }
            last = i;
            if(!swapped) {
                break;
            }
        }
    }

    template<typename Forward_Iterator>
    void bubble_sort(Forward_Iterator first, Forward_Iterator last) {
        using value_type = typename Iterator_Traits<Forward_Iterator>::value_type;
        bubble_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs == rhs; });
    }

    template<typename Random_Access_Iterator, typename Predicate>
    void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last, Predicate predicate) {
        if(first == last) {
            return;
        }

        for(Random_Access_Iterator i = first + 1; i != last; ++i) {
            for(Random_Access_Iterator j = i; j != first;) {
                Random_Access_Iterator prev = j;
                --prev;
                if(predicate(*j, *prev)) {
                    swap(*j, *prev);
                    j = prev;
                } else {
                    break;
                }
            }
        }
    }

    template<typename Random_Access_Iterator>
    void insertion_sort(Random_Access_Iterator first, Random_Access_Iterator last) {
        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
        insertion_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
    }

    template<typename Random_Access_Iterator, typename Predicate>
    void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last, Predicate predicate) {
        auto const size = last - first;
        if(size < 2) {
            return;
        }

        if(size < 6) {
            bubble_sort(first, last, predicate);
            return;
        }

        Random_Access_Iterator pivot = first + size / 2;
        Random_Access_Iterator i = first;
        Random_Access_Iterator j = last - 1;
        while(true) {
            while(predicate(*i, *pivot)) {
                ++i;
            }

            while(predicate(*pivot, *j)) {
                --j;
            }

            if(i < j) {
                swap(*i, *j);
                if(i == pivot) {
                    pivot = j;
                } else if(j == pivot) {
                    pivot = i;
                }
                ++i;
                --j;
            } else {
                break;
            }
        }

        quick_sort(first, j + 1, predicate);
        quick_sort(j + 1, last, predicate);
    }

    template<typename Random_Access_Iterator>
    void quick_sort(Random_Access_Iterator first, Random_Access_Iterator last) {
        using value_type = typename Iterator_Traits<Random_Access_Iterator>::value_type;
        quick_sort(first, last, [](value_type const& lhs, value_type const& rhs) { return lhs < rhs; });
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_ALGORITHM_HPP_INCLUDE
