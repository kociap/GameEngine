#ifndef ENGINE_OBJECT_MANAGER_HPP_INCLUDE
#define ENGINE_OBJECT_MANAGER_HPP_INCLUDE

#include "handle.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <queue>

template <typename T, typename Allocator = std::allocator<T>>
class Object_Manager {
public:
    using size_t = uint64_t;

private:
	class Deleter {
	private:
        Allocator& allocator;
        const size_t capacity;

	public:
        Deleter(Allocator& alloc, size_t cap) : allocator(alloc), capacity(cap) {}

		void operator()(T* ptr) {
            allocator.deallocate(ptr, capacity);
		}
	};

private:
    using storage_ptr = std::unique_ptr<T, Deleter>;

private:
    std::queue<size_t> free_locations;
    Allocator allocator;
    const size_t initial_storage_capacity = 64;
    size_t capacity = 0;
    size_t size = 0;
    storage_ptr storage;

public:
    Object_Manager(): storage(nullptr, Deleter(allocator, capacity)) {}
    Object_Manager(Object_Manager const& original): storage(nullptr, Deleter(allocator, capacity)) {}
    Object_Manager(Object_Manager&& from): storage(std::move(from.storage)) {}
    Object_Manager& operator=(Object_Manager const&) {
        return *this;
    }
    Object_Manager& operator=(Object_Manager&&) {
        return *this;
    }
    ~Object_Manager() {}

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
            for (size_t i = 0; i < size; ++i) {
                attempt_move(to.get() + i, from.get() + i);
            }
        } else {
            for (size_t i = 0; i < size; ++i) {
                try {
                    attempt_construct(from.get() + i, to.get() + i);
				} catch { // This catch-all is bad
					// TODO Handle errors (maybe)
                    throw; // temporarily rethrow
				}
			}
        }
    }

	void check_size() {
        if (size == capacity) {
            size_t new_capacity = (capacity == 0 ? initial_storage_capacity : capacity * 2);
            storage_ptr new_storage(nullptr, Deleter(allocator, new_capacity));
            new_storage.reset(allocator.allocate(new_capacity));
            reallocate(storage, new_storage);
            std::swap(storage, new_storage);
			capacity = new_capacity;
            for (size_t i = size; i < new_capacity; ++i) {
                free_locations.push(i);
			}
		}
	}

public:
    void resize(size_t n);
    void reserve(size_t n);

	template<typename... CtorArgs>
    Handle<T> add(CtorArgs&&... args) {
        check_size();
        size_t location = free_locations.front();
        new (storage.get() + location) T(std::forward<std::decay_t<CtorArgs>>(args)...);
        free_locations.pop();
		++size;
        return Handle<T>(location);
    }

	// Returns reference to the object which might be invalidated after reallocation
	// TODO solve this
    T& get(Handle<T> const& handle) {
        T* object = storage.get() + handle.value;
        return *object;
	}

	void remove(Handle<T> const& handle) {
        T* elem_ptr = storage.get() + handle.value;
        elem_ptr->~T();
        free_locations.push(handle.value);
        --size;
	}
};

#endif // !ENGINE_OBJECT_MANAGER_HPP_INCLUDE