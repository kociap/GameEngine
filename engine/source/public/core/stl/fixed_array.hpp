#ifndef CORE_ANTON_STL_FIXED_ARRAY_HPP_INCLUDE
#define CORE_ANTON_STL_FIXED_ARRAY_HPP_INCLUDE

#include <core/types.hpp>
#include <core/stl/tags.hpp>
#include <core/exception.hpp>
#include <core/memory/aligned_buffer.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>

namespace anton_engine::anton_stl {
    template <typename T, i64 Capacity>
    class Fixed_Array {
        static_assert(Capacity >= 0, "Fixed_Array's capacity may not be less than 0");

    public:
        using value_type = T;
        using size_type = i64;
        using difference_type = isize;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;

        Fixed_Array();
        explicit Fixed_Array(size_type size);
        Fixed_Array(Reserve_Tag, size_type size);
        Fixed_Array(size_type, value_type const&);
        Fixed_Array(Fixed_Array const& original);
        Fixed_Array(Fixed_Array&& from) noexcept;
        template<typename Input_Iterator>
        Fixed_Array(Range_Construct_Tag, Input_Iterator first, Input_Iterator last);
        template <typename... Args>
        Fixed_Array(Variadic_Construct_Tag, Args&&...);
        ~Fixed_Array();
        Fixed_Array& operator=(Fixed_Array const& original);
        Fixed_Array& operator=(Fixed_Array&& from) noexcept;

        [[nodiscard]] reference operator[](size_type index);
        [[nodiscard]] const_reference operator[](size_type index) const;
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

        void resize(size_type count);
        void resize(size_type count, T const& value);
        void clear();
        template <typename... Args>
        reference emplace_back(Args&&... args);
        void pop_back();

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> _data[Capacity];
        size_type _size;

        T* get_ptr(size_type);
        T const* get_ptr(size_type) const;
        template <typename... Args>
        void construct(void* ptr, Args&&... args);

        friend void deserialize(serialization::Binary_Input_Archive&, anton_stl::Fixed_Array<T, Capacity>&);
        friend void serialize(serialization::Binary_Output_Archive&, anton_stl::Fixed_Array<T, Capacity> const&);
        friend void swap(Fixed_Array<T, Capacity>&, Fixed_Array<T, Capacity>&);
    };
} // namespace anton_engine::anton_stl

#include <core/stl/fixed_array.tpp>

#endif // !CORE_ANTON_STL_FIXED_ARRAY_HPP_INCLUDE
