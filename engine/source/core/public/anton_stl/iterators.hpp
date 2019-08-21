#ifndef CORE_ANTON_STL_ITERATORS_HPP_INCLUDE
#define CORE_ANTON_STL_ITERATORS_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/type_traits.hpp>

namespace std {
    struct input_iterator_tag;
    struct output_iterator_tag;
    struct forward_iterator_tag;
    struct bidirectional_iterator_tag;
    struct random_access_iterator_tag;
} // namespace std

namespace anton_stl {
    template <typename T>
    struct iterator_traits {
        using difference_type = typename T::difference_type;
        using value_type = typename T::value_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
    };

    template <typename T>
    struct iterator_traits<T*> {
        using difference_type = anton_ptrdiff_t;
        using value_type = anton_stl::remove_const<T>;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
    };

    template <typename Container>
    class const_iterator;

    template <typename Container>
    class iterator {
        friend Container;
        friend class const_iterator<Container>;

    public:
        using value_type = typename Container::value_type;
        using pointer = typename Container::pointer;
        using reference = typename Container::reference;
        using difference_type = anton_ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        iterator operator++(int) {
            auto copy = *this;
            return ++(*this), copy;
        }

        iterator operator--(int) {
            auto copy = *this;
            return --(*this), copy;
        }

        iterator& operator+=(difference_type n) {
            storage_ptr += n;
            return *this;
        }

        iterator& operator-=(difference_type n) {
            storage_ptr -= n;
            return *this;
        }

        [[nodiscard]] iterator operator+(difference_type n) {
            return iterator(storage_ptr + n);
        }

        [[nodiscard]] iterator operator-(difference_type n) {
            return iterator(storage_ptr - n);
        }

        // clang-format off
        [[nodiscard]] reference operator*() const { return *storage_ptr; }
        [[nodiscard]] pointer operator->() const { return storage_ptr; }
        [[nodiscard]] reference operator[](difference_type n) const { return *(storage_ptr + n); }

        [[nodiscard]] friend iterator operator+(difference_type n, iterator const& a) { return iterator(a.storage_ptr + n); }
        [[nodiscard]] friend difference_type operator-(iterator const& a, iterator const& b) { return a.storage_ptr - b.storage_ptr; }

        [[nodiscard]] friend bool operator==(iterator const& a, iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        [[nodiscard]] friend bool operator!=(iterator const& a, iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        [[nodiscard]] friend bool operator<(iterator const& a, iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        [[nodiscard]] friend bool operator>(iterator const& a, iterator const& b) { return b < a; }
        [[nodiscard]] friend bool operator<=(iterator const& a, iterator const& b) { return !(a > b); }
        [[nodiscard]] friend bool operator>=(iterator const& a, iterator const& b) { return !(a < b); }
        // clang-format on

    private:
        pointer storage_ptr = nullptr;
        iterator(pointer ptr): storage_ptr(ptr) {}
    };

    template <typename Container>
    class const_iterator {
        friend Container;

    public:
        using value_type = typename Container::value_type;
        using pointer = typename Container::const_pointer;
        using reference = typename Container::const_reference;
        using difference_type = anton_ptrdiff_t;
        using iterator_category = std::random_access_iterator_tag;

        const_iterator(iterator<Container> iter): storage_ptr(iter.storage_ptr) {}

        const_iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        const_iterator operator++(int) {
            auto copy = *this;
            return ++(*this), copy;
        }

        const_iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        const_iterator operator--(int) {
            auto copy = *this;
            return --(*this), copy;
        }

        const_iterator& operator+=(difference_type n) {
            storage_ptr += n;
            return *this;
        }

        const_iterator& operator-=(difference_type n) {
            storage_ptr -= n;
            return *this;
        }

        [[nodiscard]] const_iterator operator+(difference_type n) {
            return const_iterator(storage_ptr + n);
        }

        [[nodiscard]] const_iterator operator-(difference_type n) {
            return const_iterator(storage_ptr - n);
        }

        // clang-format off
        [[nodiscard]] reference operator*() const { return *storage_ptr; }
        [[nodiscard]] pointer operator->() const { return storage_ptr; }
        [[nodiscard]] reference operator[](difference_type n) const { return *(storage_ptr + n); }

        [[nodiscard]] friend const_iterator operator+(difference_type n, const_iterator const& a) { return const_iterator(a.storage_ptr + n); }
        [[nodiscard]] friend difference_type operator-(const_iterator const& a, const_iterator const& b) { return a.storage_ptr - b.storage_ptr; }

        [[nodiscard]] friend bool operator==(const_iterator const& a, const_iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        [[nodiscard]] friend bool operator!=(const_iterator const& a, const_iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        [[nodiscard]] friend bool operator<(const_iterator const& a, const_iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        [[nodiscard]] friend bool operator>(const_iterator const& a, const_iterator const& b) { return b < a; }
        [[nodiscard]] friend bool operator<=(const_iterator const& a, const_iterator const& b) { return !(a > b); }
        [[nodiscard]] friend bool operator>=(const_iterator const& a, const_iterator const& b) { return !(a < b); }
        // clang-format on

    protected:
        pointer storage_ptr = nullptr;
        const_iterator(pointer ptr): storage_ptr(ptr) {}
    };

    template <typename Iterator>
    class reverse_iterator {
    public:
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using difference_type = typename Iterator::difference_type;
        using iterator_category = typename Iterator::iterator_category;
        using iterator_type = Iterator;

        explicit reverse_iterator(iterator_type iter): _iterator(--iter) {}

        iterator_type base() const {
            return _iterator;
        }

        reverse_iterator& operator++() {
            --_iterator;
            return *this;
        }

        reverse_iterator& operator--() {
            ++_iterator;
            return *this;
        }

        reverse_iterator operator++(int) {
            auto copy = *this;
            --_iterator;
            return copy;
        }

        reverse_iterator operator--(int) {
            auto copy = *this;
            ++_iterator;
            return copy;
        }

        reverse_iterator& operator+=(difference_type n) {
            _iterator -= n;
            return *this;
        }

        reverse_iterator& operator-=(difference_type n) {
            _iterator += n;
            return *this;
        }

        [[nodiscard]] reverse_iterator operator+(difference_type n) {
            return reverse_iterator(_iterator - n);
        }

        [[nodiscard]] reverse_iterator operator-(difference_type n) {
            return reverse_iterator(_iterator + n);
        }

        // clang-format off
        [[nodiscard]] reference operator*() const { return *_iterator; }
        [[nodiscard]] pointer operator->() const { return _iterator.operator->(); }
        [[nodiscard]] reference operator[](difference_type n) { return _iterator[-n]; }

        [[nodiscard]] friend reverse_iterator operator+(difference_type n, reverse_iterator const& a) { return reverse_iterator(a._iterator - n); }
        [[nodiscard]] friend difference_type operator-(reverse_iterator const& a, reverse_iterator const& b) { return b._iterator - a._iterator; }

        [[nodiscard]] friend bool operator==(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator == b._iterator; }
        [[nodiscard]] friend bool operator!=(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator != b._iterator; }
        [[nodiscard]] friend bool operator<(reverse_iterator const& a, reverse_iterator const& b) { return b._iterator - a._iterator < 0; }
        [[nodiscard]] friend bool operator>(reverse_iterator const& a, reverse_iterator const& b) { return b < a; }
        [[nodiscard]] friend bool operator<=(reverse_iterator const& a, reverse_iterator const& b) { return !(a > b); }
        [[nodiscard]] friend bool operator>=(reverse_iterator const& a, reverse_iterator const& b) { return !(a < b); }
        // clang-format on

    private:
        iterator_type _iterator;
    };
} // namespace anton_stl
#endif // !CORE_ANTON_STL_ITERATORS_HPP_INCLUDE
