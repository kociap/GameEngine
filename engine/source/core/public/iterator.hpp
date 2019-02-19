#ifndef CORE_CONTAINERS_ITERABLE_HPP_INCLUDE
#define CORE_CONTAINERS_ITERABLE_HPP_INCLUDE

namespace iterators {
    template <typename Container>
    class iterator {
        friend Container;

    public:
        using value_type = typename Container::value_type;
        using pointer = value_type*;

    private:
        pointer storage_ptr = nullptr;

        iterator(pointer ptr) : storage_ptr(ptr) {}

    public:
        iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        value_type& operator*() {
            return *storage_ptr;
        }

        pointer operator->() {
            return storage_ptr;
        }

        friend bool operator==(iterator const& a, iterator const& b) {
            return a.storage_ptr == b.storage_ptr;
        }

        friend bool operator!=(iterator const& a, iterator const& b) {
            return a.storage_ptr != b.storage_ptr;
        }

        friend bool operator<(iterator const& a, iterator const& b) {
            return b.storage_ptr - a.storage_ptr > 0;
        }

        friend bool operator>(iterator const& a, iterator const& b) {
            return b < a;
        }
    };

    template <typename Container>
    class reverse_iterator {};

    template <typename Container>
    class const_iterator {
        friend Container;

    public:
        using value_type = typename Container::value_type;
        using pointer = value_type*;

    private:
        pointer storage_ptr = nullptr;

        const_iterator(pointer ptr) : storage_ptr(ptr) {}

    public:
        const_iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        const_iterator& operator--() {
            --storage_ptr;
            return *this;
        }

        value_type const& operator*() {
            return *storage_ptr;
        }

        value_type const* operator->() {
            return storage_ptr;
        }

        friend bool operator==(const_iterator const& a, const_iterator const& b) {
            return a.storage_ptr == b.storage_ptr;
        }

        friend bool operator!=(const_iterator const& a, const_iterator const& b) {
            return a.storage_ptr != b.storage_ptr;
        }

        friend bool operator<(const_iterator const& a, const_iterator const& b) {
            return b.storage_ptr - a.storage_ptr > 0;
        }

        friend bool operator>(const_iterator const& a, const_iterator const& b) {
            return b < a;
        }
    };

    template <typename Container>
    class reverse_const_iterator {};
}; // namespace iterators

#endif // !CORE_CONTAINERS_ITERABLE_HPP_INCLUDE