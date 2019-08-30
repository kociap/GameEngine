#ifndef CORE_ANTON_STL_STRING_HPP_INCLUDE
#define CORE_ANTON_STL_STRING_HPP_INCLUDE

#include <anton_stl/allocator.hpp>
#include <anton_stl/config.hpp>
#include <anton_stl/detail/string_iterators.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/string_view.hpp>

namespace anton_stl {
    // String
    // UTF-8 encoded string
    //
    // Does not provide begin/end functions, but instead exposes 2 functions - bytes and chars.
    // Both return proxy classes which have begin/end functions that return iterators over
    // bytes and code points respectively.
    //
    // Notes:
    // operator[] is not implemented because UTF-8 doesn't allow us to index in constant-time.
    //
    // TODO: SSO
    //
    class String {
    public:
        using value_type = char;
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using allocator_type = anton_stl::Polymorphic_Allocator;
        using byte_iterator = char*;
        using byte_const_iterator = char*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        String();
        explicit String(allocator_type const&);
        String(Reserve_Tag, size_type n);
        String(Reserve_Tag, size_type n, allocator_type const&);
        // Constructs String from null-terminated UTF-8 string
        String(value_type const*);
        // Constructs String from null-terminated UTF-8 string
        String(value_type const*, allocator_type const&);
        String(value_type const*, size_type);
        String(value_type const*, size_type, allocator_type const&);
        // Does not copy allocator
        String(String const&);
        String(String const&, allocator_type const&);
        String(String&&) noexcept;
        String(String&&, allocator_type const&);
        ~String();

        String& operator=(String const&);
        String& operator=(String&&) noexcept;

    public:
        // Implicit conversion operator
        [[nodiscard]] operator String_View() const;

    public:
        [[nodiscard]] UTF8_Bytes bytes();
        [[nodiscard]] UTF8_Const_Bytes bytes() const;
        [[nodiscard]] UTF8_Const_Bytes const_bytes() const;
        [[nodiscard]] UTF8_Chars chars() const;

        [[nodiscard]] byte_iterator bytes_begin();
        [[nodiscard]] byte_const_iterator bytes_begin() const;
        [[nodiscard]] byte_const_iterator bytes_cbegin() const;

        [[nodiscard]] byte_iterator bytes_end();
        [[nodiscard]] byte_const_iterator bytes_end() const;
        [[nodiscard]] byte_const_iterator bytes_cend() const;

        // Always const
        [[nodiscard]] char_iterator chars_begin() const;
        // Always const
        [[nodiscard]] char_iterator chars_end() const;

        // TODO: Reverse iterators

        // Capacity of the string in bytes.
        [[nodiscard]] size_type capacity() const;
        // Size of the string in bytes.
        [[nodiscard]] size_type size() const;
        // Counts the number of Unicode code points.
        // This is a linear-time operation.
        [[nodiscard]] size_type size_utf8() const;

        // Allocates at least n bytes of storage.
        // Might allocate more.
        void reserve(size_type n);
        // Allocates exactly n bytes of storage.
        void reserve_exact(size_type n);
        // Changes the size of the string to n. Useful in situations when the user
        // writes to the string via external means.
        void force_size(size_type n);

        void clear();
        void append(String_View);
        // template <typename Input_Iterator>
        // iterator insert(const_iterator pos, Input_Iterator first, Input_Iterator last);

        [[nodiscard]] value_type* data();
        [[nodiscard]] value_type const* data() const;
        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        friend void swap(anton_stl::String&, anton_stl::String&);

    private:
        allocator_type _allocator;
        value_type* _data = nullptr;
        size_type _capacity = 64;
        size_type _size = 0;
    };

    // Compares bytes
    [[nodiscard]] bool operator==(String const&, String const&);

    // Compares bytes
    [[nodiscard]] inline bool operator!=(String const& lhs, String const& rhs) {
        return !(lhs == rhs);
    }

    // compare
    // Compares two strings <not implemented>
    // Returns: -1 if lhs < rhs, 0 if lhs == rhs and 1 if lhs > rhs.
    //
    // TODO: Consider creating a dedicated class for the return type
    //
    [[nodiscard]] int compare(String const& lhs, String const& rhs);

} // namespace anton_stl

namespace serialization {
    class Binary_Output_Archive;
    class Binary_Input_Archive;

    void serialize(Binary_Output_Archive&, anton_stl::String const&);
    void deserialize(Binary_Input_Archive&, anton_stl::String&);
} // namespace serialization

#endif // !CORE_ANTON_STL_STRING_HPP_INCLUDE
