#include "debug_macros.hpp"
#include "math/math.hpp"
#include "memory/memory.hpp"
#include "memory/stack_allocate.hpp"
#include "utility.hpp"

namespace containers {
    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector() {
        storage = allocator.allocate(_capacity);
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type const n): _capacity(n) {
        storage = allocator.allocate(_capacity);
        try {
            memory::uninitialized_default_construct_n(storage, n);
            _size = n;
        } catch (...) {
            allocator.deallocate(storage, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type const n, reserve_t): _capacity(n) {
        storage = allocator.allocate(_capacity);
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(size_type n, value_type const& value): _capacity(n) {
        storage = allocator.allocate(_capacity);
        try {
            memory::uninitialized_fill_n(storage, n, value);
            _size = n;
        } catch (...) {
            allocator.deallocate(storage, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector const& v): allocator(v.allocator), _capacity(v._capacity) {
        storage = allocator.allocate(_capacity);
        try {
            memory::uninitialized_copy_n(v.storage, v._size, storage);
            _size = v._size;
        } catch (...) {
            allocator.deallocate(storage, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(Vector&& v) noexcept: allocator(std::move(v.allocator)), _capacity(v._capacity), _size(v._size), storage(v.storage) {
        v.storage = nullptr;
        v._capacity = 0;
        v._size = 0;
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::Vector(std::initializer_list<T> list): _capacity(list.size()) {
        storage = allocator.allocate(_capacity);
        try {
            memory::uninitialized_move(list.begin(), list.end(), storage);
            _size = list.size();
        } catch (...) {
            allocator.deallocate(storage, _capacity);
            throw;
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>::~Vector() {
        if (storage != nullptr) {
            memory::destruct_n(storage, _size);
            allocator.deallocate(storage, _capacity);
        }
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector const& v) {
        static_assert(std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value, "Allocator is not copy assignable");
        allocator = v.allocator;
        T* new_storage = allocator.allocate(v._capacity);
        try {
            memory::uninitialized_copy_n(v.storage, v._size, new_storage);
        } catch (...) {
            allocator.deallocate(new_storage, v._capacity);
            throw;
        }
        memory::destruct_n(storage, _size);
        allocator.deallocate(storage, _capacity); // TODO assumes v.allocator == allocator is true
        storage = new_storage;
        _size = v._size;
        _capacity = v._capacity;
        return *this;
    }

    template <typename T, typename Allocator>
    Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& v) noexcept {
        GE_assert(std::allocator_traits<Allocator>::propagate_on_container_swap::value || allocator == v.allocator, "Allocator not swappable");
        swap(storage, v.storage);
        swap(allocator, v.allocator); // TODO uses swap instead of move assignment
        swap(_capacity, v._capacity);
        swap(_size, v._size);
        return *this;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::at(size_type index) {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at(size_type index) const {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::at_unchecked(size_type index) {
        T* object = get_ptr(index);
        return *object;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::at_unchecked(size_type index) const {
        T* object = get_ptr(index);
        return *object;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::operator[](size_type index) {
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::operator[](size_type index) const {
        return at_unchecked(index);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::front() {
        return at_unchecked(0);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::front() const {
        return at_unchecked(0);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::back() {
        return at_unchecked(_size - 1);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reference Vector<T, Allocator>::back() const {
        return at_unchecked(_size - 1);
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::pointer Vector<T, Allocator>::data() {
        return get_ptr();
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_pointer Vector<T, Allocator>::data() const {
        return get_ptr();
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::begin() {
        return iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::iterator Vector<T, Allocator>::end() {
        return iterator(get_ptr(_size));
    }
    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::begin() const {
        return const_iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::end() const {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cbegin() const {
        return const_iterator(get_ptr());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_iterator Vector<T, Allocator>::cend() const {
        return const_iterator(get_ptr(_size));
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rbegin() {
        return reverse_iterator(end());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::reverse_iterator Vector<T, Allocator>::rend() {
        return reverse_iterator(begin());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crbegin() const {
        return const_reverse_iterator(cend());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::const_reverse_iterator Vector<T, Allocator>::crend() const {
        return const_reverse_iterator(cbegin());
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::size() const {
        return _size;
    }

    template <typename T, typename Allocator>
    typename Vector<T, Allocator>::size_type Vector<T, Allocator>::capacity() const {
        return _capacity;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type n, value_type const& value) {
        if (n > _capacity) {
            grow(n); // TODO apply growing policy here?
        }

        if (n > _size) {
            memory::uninitialized_fill(get_ptr(_size), get_ptr(n), value);
        } else {
            memory::destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::resize(size_type n) {
        if (n > _capacity) {
            grow(n); // TODO apply growing policy here?
        }

        if (n > _size) {
            memory::uninitialized_default_construct(get_ptr(_size), get_ptr(n));
        } else {
            memory::destruct(get_ptr(n), get_ptr(_size));
        }
        _size = n;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::reserve(size_type n) {
        if (n > _capacity) {
            grow(n);
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::set_capacity(size_type n) {
        if (n > _capacity) { // Expected to happen more often
            grow(n);
        } else if (n < _capacity) {
            shrink(n);
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::shrink_to_fit() {
        if (_size < _capacity) {
            shrink(_size);
        }
    }

    template <typename T, typename Allocator>
    template <typename Input_Iterator>
    void Vector<T, Allocator>::assign(Input_Iterator first, Input_Iterator last) {
        iterator my_first = begin();
        for (iterator my_end = end(); my_first != my_end && first != last; ++my_first, ++first) {
            *my_first = *first;
        }

        // TODO insert other elements if first != last or erase elements past my_first if first == last
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::insert_unsorted(const_iterator position, value_type const& value) {
        GE_assert(position.storage_ptr >= get_ptr() && position.storage_ptr <= get_ptr(_size), "Vector::insert invalid iterator");

        size_type offset = position.storage_ptr - storage;
        check_size();
        if (offset == _size) {
            attempt_construct(get_ptr(offset), value);
            ++_size;
        } else {
            T* elem_ptr = get_ptr(offset);
            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                attempt_move(elem_ptr, get_ptr(_size));
            } else {
                attempt_copy(elem_ptr, get_ptr(_size));
            }
            memory::destruct(elem_ptr);
            attempt_construct(elem_ptr, value);
            ++_size;
        }
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(value_type const& val) {
        check_size();
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, val);
        ++_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::push_back(value_type&& val) {
        check_size();
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, std::move(val));
        ++_size;
    }

    template <typename T, typename Allocator>
    template <typename... CtorArgs>
    typename Vector<T, Allocator>::reference Vector<T, Allocator>::emplace_back(CtorArgs&&... args) {
        check_size();
        T* elem_ptr = get_ptr(_size);
        attempt_construct(elem_ptr, std::forward<CtorArgs>(args)...);
        ++_size;
        return *elem_ptr;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted(size_type index) {
        if (index >= _size) {
            throw std::out_of_range("index out of range");
        }

        erase_unsorted_unchecked(index);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::erase_unsorted_unchecked(size_type index) {
        T* elem_ptr = get_ptr(index);
        T* last_elem_ptr = get_ptr(_size - 1);
        *elem_ptr = std::move(*last_elem_ptr);
        memory::destruct(last_elem_ptr);
        --_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::pop_back() {
        T* last_elem_ptr = get_ptr(_size - 1);
        memory::destruct(last_elem_ptr);
        --_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::clear() {
        memory::destruct(get_ptr(), get_ptr(_size));
        _size = 0;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_copy(T* from, T* to) {
        ::new (to) T(*from);
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::attempt_move(T* from, T* to) {
        ::new (to) T(std::move(*from));
    }

    template <typename T, typename Allocator>
    template <typename... Ctor_Args>
    void Vector<T, Allocator>::attempt_construct(T* in, Ctor_Args&&... args) {
        if constexpr (std::is_constructible_v<T, Ctor_Args&&...>) {
            ::new (in) T(std::forward<Ctor_Args>(args)...);
        } else {
            ::new (in) T{std::forward<Ctor_Args>(args)...};
        }
    }

    template <typename T, typename Allocator>
    T* Vector<T, Allocator>::get_ptr(size_type index) const {
        return storage + index;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::move_contents(T* const& from, T* const& to, size_type number_of_elements_to_copy) {
        // TODO
        if constexpr (std::is_nothrow_move_constructible_v<T> /* || !std::is_copy_constructible_v<T> */) {
            memory::uninitialized_move_n(from, number_of_elements_to_copy, to);
        } else {
            memory::uninitialized_copy_n(from, number_of_elements_to_copy, to);
        }
    }

    // TODO remove? is essentially shrink, but with less functionality
    template <typename T, typename Allocator>
    void Vector<T, Allocator>::grow(size_type new_capacity) {
        // Assumes new_capacity >= _capacity
        T* new_storage = allocator.allocate(new_capacity);
        try {
            move_contents(storage, new_storage, _size);
        } catch (...) {
            allocator.deallocate(new_storage, _capacity);
            throw;
        }
        swap(storage, new_storage);
        memory::destruct_n(new_storage, _size);
        allocator.deallocate(new_storage, _capacity);
        _capacity = new_capacity;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::shrink(size_type new_capacity) {
        T* new_storage = allocator.allocate(new_capacity);
        size_type min_cap_size = math::min(new_capacity, _size);
        try {
            move_contents(storage, new_storage, min_cap_size);
        } catch (...) {
            allocator.deallocate(new_storage, new_capacity);
            throw;
        }
        swap(storage, new_storage);
        memory::destruct_n(new_storage, _size);
        _capacity = new_capacity;
        _size = min_cap_size;
    }

    template <typename T, typename Allocator>
    void Vector<T, Allocator>::check_size() {
        if (_size == _capacity) {
            size_type new_capacity = _capacity * 2;
            grow(new_capacity);
        }
    }
} // namespace containers

namespace serialization {
    template <typename T, typename Allocator>
    void serialize(Binary_Output_Archive& out, containers::Vector<T, Allocator> const& vec) {
        using size_type = typename containers::Vector<T, Allocator>::size_type;
        size_type capacity = vec.capacity(), size = vec.size();
        out.write(capacity);
        out.write(size);
        for (T const& elem: vec) {
            serialization::serialize(out, elem);
        }
    }

    template <typename T, typename Allocator>
    void deserialize(Binary_Input_Archive& in, containers::Vector<T, Allocator>& vec) {
        using size_type = typename containers::Vector<T, Allocator>::size_type;
        size_type capacity, size;
        in.read(capacity);
        in.read(size);
        vec.clear();
        vec.set_capacity(capacity);
        if constexpr (std::is_default_constructible_v<T>) {
            vec.resize(size);
            try {
                for (T& elem: vec) {
                    serialization::deserialize(in, elem);
                }
            } catch (...) {
                // TODO move stream backward to maintain weak guarantee
                memory::destruct_n(vec.get_ptr(), size);
                throw;
            }
        } else {
            size_type n = size;
            try {
                for (; n > 0; --n) {
                    Stack_Allocate<T> elem;
                    vec.push_back(std::move(elem.reference()));
                    serialization::deserialize(in, vec.back());
                }
                vec._size = size;
            } catch (...) {
                // TODO move stream backward to maintain weak guarantee
                memory::destruct_n(vec.get_ptr(), size - n);
                throw;
            }
        }
    }
} // namespace serialization