#ifndef CORE_ATL_VECTOR_HPP_INCLUDE
#define CORE_ATL_VECTOR_HPP_INCLUDE

#include <core/assert.hpp>
#include <core/atl/allocator.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/memory.hpp>
#include <core/atl/tags.hpp>
#include <core/atl/type_traits.hpp>
#include <core/atl/utility.hpp>
#include <core/math/math.hpp>
#include <core/memory/stack_allocate.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>

#include <stdexcept> // includes std::allocator
#include <type_traits>

namespace anton_engine::atl {
    template <typename T, typename Allocator = atl::Allocator>
    class Vector {
        static_assert(std::is_nothrow_move_constructible_v<T> || std::is_copy_constructible_v<T> || std::is_trivially_copy_constructible_v<T>,
                      "Type is neither nothrow move constructible nor copy constructible");

    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = i64;
        using difference_type = isize;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;

        Vector();
        explicit Vector(size_type size);
        Vector(Reserve_Tag, size_type size);
        Vector(size_type, value_type const&);
        Vector(Vector const& original);
        Vector(Vector&& from) noexcept;
        template<typename Input_Iterator>
        Vector(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template <typename... Args>
        Vector(Variadic_Construct_Tag, Args&&...);
        ~Vector();
        Vector& operator=(Vector const& original);
        Vector& operator=(Vector&& from) noexcept;

        [[nodiscard]] reference operator[](size_type);
        [[nodiscard]] const_reference operator[](size_type) const;
        [[nodiscard]] pointer data();
        [[nodiscard]] const_pointer data() const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] const_iterator cend() const;

        [[nodiscard]] size_type size() const;
        [[nodiscard]] size_type capacity() const;

        void resize(size_type n);
        void resize(size_type n, value_type const&);
        // Allocates at least n bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        void reserve(size_type n);
        void set_capacity(size_type n);
        // Changes the size of the vector to n. Useful in situations when the user
        // writes to the vector via external means.
        void force_size(size_type n);

        template <typename Input_Iterator>
        void assign(Input_Iterator first, Input_Iterator last);
        void insert(size_type position, value_type const&);
        // void insert(const_iterator position, value_type const& value);
        void insert_unsorted(const_iterator position, value_type const& value);
        void push_back(value_type const&);
        void push_back(value_type&&);
        template <typename... CtorArgs>
        reference emplace_back(CtorArgs&&... args);
        void erase_unsorted(size_type index); // TODO: remove
        void erase_unsorted_unchecked(size_type index);
        iterator erase_unsorted(const_iterator first);
        // iterator erase_unsorted(const_iterator first, const_iterator last);
        iterator erase(const_iterator first, const_iterator last);
        void pop_back();
        void clear();

    private:
        Allocator _allocator;
        size_type _capacity = 64;
        size_type _size = 0;
        T* _data = nullptr;

    private:
        void attempt_copy(T* from, T* to);
        void attempt_move(T* from, T* to);
        template <typename... Ctor_Args>
        void attempt_construct(T* in, Ctor_Args&&... args);

        T* get_ptr(size_type index = 0);
        T const* get_ptr(size_type index = 0) const;

        T* allocate(size_type);
        void deallocate(void*, size_type);
        void ensure_capacity(size_type requested_capacity);
    };

} // namespace anton_engine::atl

namespace anton_engine {
    template <typename T, typename Allocator>
    void serialize(serialization::Binary_Output_Archive&, atl::Vector<T, Allocator> const&);
    template <typename T, typename Allocator>
    void deserialize(serialization::Binary_Input_Archive&, atl::Vector<T, Allocator>&);
} // namespace anton_engine

#include <core/atl/vector.tpp>

#endif // !CORE_ATL_VECTOR_HPP_INCLUDE
