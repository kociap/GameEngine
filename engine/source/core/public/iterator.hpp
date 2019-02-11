#ifndef CORE_CONTAINERS_ITERABLE_HPP_INCLUDE
#define CORE_CONTAINERS_ITERABLE_HPP_INCLUDE

namespace iterators {
    template <typename Container>
    class iterator {
    private:
        friend Container;

        using pointer = typename Container::pointer;
        using value_type = typename Container::value_type;

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
    class const_iterator {};

    template <typename Container>
    class reverse_const_iterator {};
}; // namespace iterators

#endif // !CORE_CONTAINERS_ITERABLE_HPP_INCLUDE