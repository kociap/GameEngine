#ifndef CORE_ATL_FIXED_ARRAY_HPP_INCLUDE
#define CORE_ATL_FIXED_ARRAY_HPP_INCLUDE

#include <core/types.hpp>
#include <core/atl/tags.hpp>
#include <core/exception.hpp>
#include <core/memory/aligned_buffer.hpp>
#include <core/memory/stack_allocate.hpp>
#include <core/serialization/archives/binary.hpp>
#include <core/serialization/serialization.hpp>
#include <core/assert.hpp>
#include <core/atl/memory.hpp>
#include <core/atl/type_traits.hpp>

#include <new> // std::launder

namespace anton_engine::atl {
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

        friend void swap(Fixed_Array& a1, Fixed_Array& a2) {
            Fixed_Array tmp = move(a1);
            a1 = move(a2);
            a2 = move(tmp);
        }

        friend void deserialize(serialization::Binary_Input_Archive& in, Fixed_Array& array) {
            using size_type = typename Fixed_Array::size_type;
            size_type capacity, size;
            in.read(capacity);
            in.read(size);
            array.clear();
            if constexpr (std::is_default_constructible_v<T>) {
                array.resize(size);
                try {
                    for (T& elem: array) {
                        deserialize(in, elem);
                    }
                } catch (...) {
                    // TODO move stream backward to maintain weak guarantee
                    atl::destruct_n(array.data(), size);
                    throw;
                }
            } else {
                size_type n = size;
                try {
                    for (; n > 0; --n) {
                        Stack_Allocate<T> elem;
                        array.push_back(std::move(elem.reference()));
                        deserialize(in, array.back());
                    }
                    array._size = size;
                } catch (...) {
                    // TODO move stream backward to maintain weak guarantee
                    atl::destruct_n(array.data(), size - n);
                    throw;
                }
            }
        }

        friend void serialize(serialization::Binary_Output_Archive& out, Fixed_Array const& array) {
            using size_type = typename Fixed_Array::size_type;
            size_type const capacity = array.capacity(), size = array.size();
            out.write(capacity);
            out.write(size);
            for (T const& elem: array) {
                serialize(out, elem);
            }
        }
    };
} // namespace anton_engine::atl

#include <core/atl/fixed_array.tpp>

#endif // !CORE_ATL_FIXED_ARRAY_HPP_INCLUDE
