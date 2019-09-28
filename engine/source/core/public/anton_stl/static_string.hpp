#ifndef CORE_ANTON_STL_STATIC_STRING_HPP_INCLUDE
#define CORE_ANTON_STL_STATIC_STRING_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/memory>
#include <anton_stl/string_view.hpp>
#include <memory/aligned_buffer.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>
#include <stdexcept>

namespace anton_engine::anton_stl {
    template <typename T, anton_stl::ssize_t Capacity>
    class Basic_Static_String {
        static_assert(Capacity >= 0, "Basic_Static_String's capacity may not be less than 0");

    public:
        // allocator_type not provided
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;
        using iterator = T*;
        using const_iterator = T const*;
        using reverse_iterator = anton_stl::Reverse_Iterator<iterator>;
        using reverse_const_iterator = anton_stl::Reverse_Iterator<const_iterator>;

    public:
        // Implicit conversion operator
        [[nodiscard]] operator anton_stl::Basic_String_View<T>() const;

    public:
        [[nodiscard]] reference operator[](size_type index);
        [[nodiscard]] const_reference operator[](size_type index) const;

        [[nodiscard]] iterator begin();
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator cbegin() const;
        [[nodiscard]] iterator end();
        [[nodiscard]] const_iterator end() const;
        [[nodiscard]] const_iterator cend() const;
        [[nodiscard]] reverse_iterator rbegin();
        [[nodiscard]] reverse_const_iterator rbegin() const;
        [[nodiscard]] reverse_const_iterator crbegin() const;
        [[nodiscard]] reverse_iterator rend();
        [[nodiscard]] reverse_const_iterator rend() const;
        [[nodiscard]] reverse_const_iterator crend() const;

        [[nodiscard]] size_type size() const;
        [[nodiscard]] constexpr size_type max_size() const;
        [[nodiscard]] constexpr size_type capacity() const;
        void resize(size_type, value_type = value_type());
        // Moves the end position to the given location. Allows the user to write to string using external means while avoiding overhead of resize.
        void force_size(size_type);

        [[nodiscard]] pointer data();
        [[nodiscard]] const_pointer data() const;
        [[nodiscard]] const_pointer c_str() const;

    private:
        Aligned_Buffer<sizeof(T), alignof(T)> storage[Capacity + 1]; // Null terimnator
        size_type _size;

        T* get_ptr();
        T const* get_ptr() const;
    };

    template <anton_size_t Capacity>
    using Static_String = Basic_Static_String<char, Capacity>;
} // namespace anton_engine::anton_stl

namespace anton_engine {
    template <typename T, anton_size_t Capacity>
    void serialize(Binary_Output_Archive&, anton_stl::Basic_Static_String<T, Capacity> const&);
    template <typename T, anton_size_t Capacity>
    void deserialize(Binary_Input_Archive&, anton_stl::Basic_Static_String<T, Capacity>&);
} // namespace anton_engine

#include <anton_stl/static_string.tpp>

#endif // !CORE_ANTON_STL_STATIC_STRING_HPP_INCLUDE
