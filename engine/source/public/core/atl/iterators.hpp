#ifndef CORE_ATL_ITERATORS_HPP_INCLUDE
#define CORE_ATL_ITERATORS_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/detail/memory_common.hpp>
#include <core/atl/type_traits.hpp>

namespace anton_engine::atl {
    // Iterator category tags
    // Explicit constructors so that the tags may not be constructed via {}

    struct Input_Iterator_Tag {
        explicit Input_Iterator_Tag() = default;
    };

    struct Output_Iterator_Tag {
        explicit Output_Iterator_Tag() = default;
    };

    struct Forward_Iterator_Tag: public Input_Iterator_Tag {
        explicit Forward_Iterator_Tag() = default;
    };

    struct Bidirectional_Input_Tag: public Forward_Iterator_Tag {
        explicit Bidirectional_Input_Tag() = default;
    };

    struct Random_Access_Iterator_Tag: public Bidirectional_Input_Tag {
        explicit Random_Access_Iterator_Tag() = default;
    };

    // Contiguous_Iterator_Tag
    // Denotes an iterator whose elements are physically adjacent in memory.
    //
    struct Contiguous_Iterator_Tag: public Random_Access_Iterator_Tag {
        explicit Contiguous_Iterator_Tag() = default;
    };

    // Is_Iterator_Category
    //
    template <typename T, typename Category>
    struct Is_Iterator_Category: public atl::Is_Convertible<T, Category> {};

    template <typename T, typename Category>
    inline constexpr bool is_iterator_category = Is_Iterator_Category<T, Category>::value;

    // Is_Iterator_Wrapper
    // Checks whether a type is an iterator wrapper. Relies on the class daclaring wrapped_iterator_type alias.
    //
    template <typename T, typename = void>
    struct Is_Iterator_Wrapper: atl::False_Type {};

    template <typename T>
    struct Is_Iterator_Wrapper<T, atl::void_trait<typename T::wrapped_iterator_type>>: atl::True_Type {};

    template <typename T>
    inline constexpr bool is_iterator_wrapper = Is_Iterator_Wrapper<T>::value;

    // Iterator_Traits
    //
    template <typename T>
    struct Iterator_Traits {
        using difference_type = typename T::difference_type;
        using value_type = typename T::value_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
    };

    template <typename T>
    struct Iterator_Traits<T*> {
        using difference_type = isize;
        using value_type = atl::remove_const<T>;
        using pointer = T*;
        using reference = T&;
        using iterator_category = atl::Contiguous_Iterator_Tag;
    };

    // TODO: Fix this
    template <typename Iterator>
    class Reverse_Iterator {
    public:
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using difference_type = typename Iterator::difference_type;
        using iterator_category = typename Iterator::iterator_category;
        using wrapped_iterator_type = Iterator;

        explicit Reverse_Iterator(wrapped_iterator_type iter): _iterator(--iter) {} // This

        wrapped_iterator_type base() const {
            return _iterator + 1; // And this
        }

        Reverse_Iterator& operator++() {
            --_iterator;
            return *this;
        }

        Reverse_Iterator& operator--() {
            ++_iterator;
            return *this;
        }

        Reverse_Iterator operator++(int) {
            auto copy = *this;
            --_iterator;
            return copy;
        }

        Reverse_Iterator operator--(int) {
            auto copy = *this;
            ++_iterator;
            return copy;
        }

        Reverse_Iterator& operator+=(difference_type n) {
            _iterator -= n;
            return *this;
        }

        Reverse_Iterator& operator-=(difference_type n) {
            _iterator += n;
            return *this;
        }

        [[nodiscard]] Reverse_Iterator operator+(difference_type n) {
            return Reverse_Iterator(_iterator - n);
        }

        [[nodiscard]] Reverse_Iterator operator-(difference_type n) {
            return Reverse_Iterator(_iterator + n);
        }

        [[nodiscard]] reference operator*() const {
            return *_iterator;
        }

        [[nodiscard]] wrapped_iterator_type operator->() const {
            return _iterator;
        }

        [[nodiscard]] reference operator[](difference_type n) const {
            return *(*this + n);
        }

    private:
        wrapped_iterator_type _iterator;
    };

    template <typename T>
    [[nodiscard]] inline constexpr Reverse_Iterator<T> operator+(typename Reverse_Iterator<T>::difference_type n, Reverse_Iterator<T> const& a) {
        return Reverse_Iterator<T>(a.base() - n);
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr auto operator-(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) -> decltype(b.base() - a.base()) {
        return b.base() - a.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator==(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() == b.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator!=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() != b.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() > b.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() < b.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator<=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() >= b.base();
    }

    template <typename T1, typename T2>
    [[nodiscard]] inline constexpr bool operator>=(Reverse_Iterator<T1> const& a, Reverse_Iterator<T2> const& b) {
        return a.base() <= b.base();
    }
} // namespace anton_engine::atl
#endif // !CORE_ATL_ITERATORS_HPP_INCLUDE
