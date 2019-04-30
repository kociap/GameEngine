#ifndef CORE_CONTAINERS_ITERATORS_HPP_INCLUDE
#define CORE_CONTAINERS_ITERATORS_HPP_INCLUDE

#include <cstddef>

namespace std {
    struct forward_iterator_tag;
    struct bidirectional_iterator_tag;
    struct random_access_iterator_tag;
} // namespace std

namespace iterators {
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
        using difference_type = std::ptrdiff_t;
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

        iterator operator+(difference_type n) {
            return iterator(storage_ptr + n);
        }

        iterator operator-(difference_type n) {
            return iterator(storage_ptr - n);
        }

        // clang-format off
        reference operator*() const { return *storage_ptr; }
        pointer operator->() const { return storage_ptr; }
        reference operator[](difference_type n) const { return *(storage_ptr + n); }

        friend iterator operator+(difference_type n, iterator const& a) { return iterator(a.storage_ptr + n); }
        friend difference_type operator-(iterator const& a, iterator const& b) { return a.storage_ptr - b.storage_ptr; }

        friend bool operator==(iterator const& a, iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        friend bool operator!=(iterator const& a, iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        friend bool operator<(iterator const& a, iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        friend bool operator>(iterator const& a, iterator const& b) { return b < a; }
        friend bool operator<=(iterator const& a, iterator const& b) { return !(a > b); }
        friend bool operator>=(iterator const& a, iterator const& b) { return !(a < b); }
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
        using difference_type = std::ptrdiff_t;
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

        const_iterator operator+(difference_type n) {
            return const_iterator(storage_ptr + n);
        }

        const_iterator operator-(difference_type n) {
            return const_iterator(storage_ptr - n);
        }

        // clang-format off
        reference operator*() const { return *storage_ptr; }
        pointer operator->() const { return storage_ptr; }
        reference operator[](difference_type n) const { return *(storage_ptr + n); }

        friend const_iterator operator+(difference_type n, const_iterator const& a) { return const_iterator(a.storage_ptr + n); }
        friend difference_type operator-(const_iterator const& a, const_iterator const& b) { return a.storage_ptr - b.storage_ptr; }

        friend bool operator==(const_iterator const& a, const_iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        friend bool operator!=(const_iterator const& a, const_iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        friend bool operator<(const_iterator const& a, const_iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        friend bool operator>(const_iterator const& a, const_iterator const& b) { return b < a; }
        friend bool operator<=(const_iterator const& a, const_iterator const& b) { return !(a > b); }
        friend bool operator>=(const_iterator const& a, const_iterator const& b) { return !(a < b); }
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

        reverse_iterator operator+(difference_type n) {
            return reverse_iterator(_iterator - n);
        }

        reverse_iterator operator-(difference_type n) {
            return reverse_iterator(_iterator + n);
        }

        // clang-format off
        reference operator*() const { return *_iterator; }
        pointer operator->() const { return _iterator.operator->(); }
        reference operator[](difference_type n) { return _iterator[-n]; }

        friend reverse_iterator operator+(difference_type n, reverse_iterator const& a) { return reverse_iterator(a._iterator - n); }
        friend difference_type operator-(reverse_iterator const& a, reverse_iterator const& b) { return b._iterator - a._iterator; }

        friend bool operator==(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator == b._iterator; }
        friend bool operator!=(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator != b._iterator; }
        friend bool operator<(reverse_iterator const& a, reverse_iterator const& b) { return b._iterator - a._iterator < 0; }
        friend bool operator>(reverse_iterator const& a, reverse_iterator const& b) { return b < a; }
        friend bool operator<=(reverse_iterator const& a, reverse_iterator const& b) { return !(a > b); }
        friend bool operator>=(reverse_iterator const& a, reverse_iterator const& b) { return !(a < b); }
        // clang-format on

    private:
        iterator_type _iterator;
    };
} // namespace iterators
#endif // !CORE_CONTAINERS_ITERATORS_HPP_INCLUDE