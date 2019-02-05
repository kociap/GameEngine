#ifndef CORE_CONTAINERS_SWAPPING_POOL_HPP_INCLUDE
#define CORE_CONTAINERS_SWAPPING_POOL_HPP_INCLUDE

#include "handle.hpp"
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Swapping_Pool {
    static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T>,
                  "Type is neither nothrow move constructible nor copy constructible");

public:
    using value_type = T;
    using size_t = uint64_t;

    class iterator {
        friend class Swapping_Pool;

    private:
        T* storage_ptr;

    private:
        iterator(T* ptr) : storage_ptr(ptr) {}

    public:
        iterator& operator++() {
            ++storage_ptr;
            return *this;
        }

        bool operator!=(iterator const& it) {
            return storage_ptr != it.storage_ptr;
        }

        T& operator*() {
            return *storage_ptr;
        }
    };

private:
    class Deleter {
    private:
        Allocator* allocator;
        size_t capacity;

    public:
        Deleter(Allocator& alloc, size_t cap) : allocator(&alloc), capacity(cap) {}
        Deleter(Deleter&) = default;
        Deleter(Deleter&&) = default;
        Deleter& operator=(Deleter& d) = default;
        Deleter& operator=(Deleter&&) = default;
        ~Deleter() {}

        void operator()(T* ptr) {
            allocator->deallocate(ptr, capacity);
        }
    };

    using storage_ptr = std::unique_ptr<T, Deleter>;

private:
    Allocator allocator;
    size_t capacity = 64;
    size_t elem_count = 0;
    storage_ptr storage;

public:
    Swapping_Pool() : storage(nullptr, Deleter(allocator, capacity)) {
        storage.reset(allocator.allocate(capacity));
    }

    // Allocates storage for cap elements
    // Does not construct elements
    Swapping_Pool(size_t cap) : capacity(cap), storage(nullptr, Deleter(allocator, cap)) {
        storage.reset(allocator.allocate(cap));
    }

    Swapping_Pool(Swapping_Pool const& original) : storage(nullptr, Deleter(allocator, capacity)) {
        throw std::runtime_error("Not implemented");
    }

    Swapping_Pool(Swapping_Pool&& from) noexcept
        : storage(std::move(from.storage)), allocator(std::move(from.allocator)), capacity(std::move(from.capacity)), elem_count(std::move(from.elem_count)) {}

    Swapping_Pool& operator=(Swapping_Pool const& original) {
        throw std::runtime_error("Not implemented");
        return *this;
    }

    Swapping_Pool& operator=(Swapping_Pool&& from) noexcept {
        storage = std::move(from.storage);
        storage = std::move(from.storage);
        allocator = std::move(from.allocator);
        capacity = std::move(from.capacity);
        elem_count = std::move(from.elem_count);
        return *this;
    }

    ~Swapping_Pool() {}

public:
    void resize(size_t n);
    void reserve(size_t n);

    template <typename... CtorArgs>
    T& add(CtorArgs&&... args) {
        check_size();
        new (storage.get() + elem_count) T(std::forward<std::decay_t<CtorArgs>>(args)...);
        T& elem = *(storage.get() + elem_count);
        ++elem_count;
        return elem;
    }

    // Returns reference to the object which might be invalidated after reallocation
    // TODO solve this
    T& get(size_t index) {
        if (index >= elem_count) {
            throw std::invalid_argument("index out of range");
        }

        return get_unchecked(index);
    }

    T& get_unchecked(size_t index) {
        T* object = storage.get() + index;
        return *object;
    }

    void remove(size_t index) {
        if (index >= elem_count) {
            throw std::invalid_argument("index out of range");
        }

        remove_unchecked(index);
    }

    void remove_unchecked(size_t index) {
        T* array_ptr = storage.get();
        T* elem_ptr = array_ptr + index;
        T* last_elem_ptr = array_ptr + elem_count - 1;
        // if constexpr (std::is_nothrow_move_constructible_v<T>) {
        static_assert(std::is_nothrow_move_constructible_v<T>, "Type is not nothrow move constructible");
        elem_ptr->~T();
        if (elem_ptr != last_elem_ptr) {
            attempt_move(last_elem_ptr, elem_ptr);
            last_elem_ptr->~T();
        }
        // } else {
        //     static_assert(std::is_copy_constructible_v<T>, "Type is neither nothrow move constructible nor copy constructible");

        //     T copy(*elem_ptr);
        //     try {
        //         elem_ptr->~T();
        //         attempt_construct(last_elem_ptr, elem_ptr);
        //     } catch {  // This catch-all is bad
        //                // TODO Handle errors (maybe)
        //         throw; // temporarily rethrow
        //     }
        // }
        --elem_count;
    }

    size_t size() {
        return elem_count;
    }

    iterator begin() {
        return iterator(storage.get());
    }

    iterator end() {
        return iterator(storage.get() + elem_count);
    }

private:
    void attempt_construct(T* from, T* to) {
        new (to) T(*from);
    }

    void attempt_move(T* from, T* to) {
        new (to) T(std::move(*from));
    }

    // Reallocates current storage
    // Uses move constructor if T is nothrow move constructible
    // Otherwise copies all elements
    void reallocate(storage_ptr const& from, storage_ptr const& to) {
        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            for (size_t i = 0; i < elem_count; ++i) {
                attempt_move(from.get() + i, to.get() + i);
            }
        } else {
            for (size_t i = 0; i < elem_count; ++i) {
                try {
                    attempt_construct(from.get() + i, to.get() + i);
                } catch (...) { // This catch-all is bad
                                // TODO Handle errors (maybe)
                    throw;      // temporarily rethrow
                }
            }
        }
    }

    void check_size() {
        if (elem_count == capacity) {
            size_t new_capacity = capacity * 2;
            storage_ptr new_storage(nullptr, Deleter(allocator, new_capacity));
            new_storage.reset(allocator.allocate(new_capacity));
            reallocate(storage, new_storage);
            std::swap(storage, new_storage);
            capacity = new_capacity;
        }
    }
};

#endif // !CORE_CONTAINERS_POOL_HPP_INCLUDE