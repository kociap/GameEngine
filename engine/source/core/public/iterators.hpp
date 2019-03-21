#ifndef CORE_CONTAINERS_ITERATORS_HPP_INCLUDE
#define CORE_CONTAINERS_ITERATORS_HPP_INCLUDE

namespace iterators {
    template <typename Container>
    class iterator {
        friend Container;

    public:
        using value_type = typename Container::value_type;
        using pointer = value_type*;
        using reference = value_type&;

        iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        reference operator*() const { return *storage_ptr; }
        pointer operator->() const { return storage_ptr; }
        friend bool operator==(iterator const& a, iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        friend bool operator!=(iterator const& a, iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        friend bool operator<(iterator const& a, iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        friend bool operator>(iterator const& a, iterator const& b) { return b < a; }

    private:
        pointer storage_ptr = nullptr;

        iterator(pointer ptr) : storage_ptr(ptr) {}
    };

    template <typename Container>
    class const_iterator {
        friend Container;

    public:
        using value_type = typename Container::value_type;
        using pointer = value_type const*;
        using reference = value_type const&;

        const_iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        const_iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        reference operator*() const { return *storage_ptr; }
        pointer operator->() const { return storage_ptr; }
        friend bool operator==(const_iterator const& a, const_iterator const& b) { return a.storage_ptr == b.storage_ptr; }
        friend bool operator!=(const_iterator const& a, const_iterator const& b) { return a.storage_ptr != b.storage_ptr; }
        friend bool operator<(const_iterator const& a, const_iterator const& b) { return b.storage_ptr - a.storage_ptr > 0; }
        friend bool operator>(const_iterator const& a, const_iterator const& b) { return b < a; }

    private:
        pointer storage_ptr = nullptr;

        const_iterator(pointer ptr) : storage_ptr(ptr) {}
    };

    template <typename Iterator>
    class reverse_iterator {
    public:
        using value_type = typename Iterator::value_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using iterator_type = Iterator;

        explicit reverse_iterator(iterator_type iter) : _iterator(--iter) {}

        reverse_iterator& operator++() {
            --_iterator;
            return *this;
        }

        reverse_iterator& operator--() {
            ++_iterator;
            return *this;
        }

        reference operator*() const { return *_iterator; }
        pointer operator->() const { return _iterator.operator->(); }
        friend bool operator==(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator == b._iterator; }
        friend bool operator!=(reverse_iterator const& a, reverse_iterator const& b) { return a._iterator != b._iterator; }
        friend bool operator<(reverse_iterator const& a, reverse_iterator const& b) { return b._iterator - a._iterator > 0; }
        friend bool operator>(reverse_iterator const& a, reverse_iterator const& b) { return b < a; }

    private:
        iterator_type _iterator;
    };
}; // namespace iterators

#endif // !CORE_CONTAINERS_ITERATORS_HPP_INCLUDE