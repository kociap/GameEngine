#ifndef CORE_ATL_STRING_HPP_INCLUDE
#define CORE_ATL_STRING_HPP_INCLUDE

#include <core/atl/allocator.hpp>
#include <core/atl/detail/string_common.hpp>
#include <core/atl/functors.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/string_view.hpp>
#include <core/atl/tags.hpp>

namespace anton_engine::atl {
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
    // TODO: Grapheme Clusters
    //
    class String {
    public:
        using value_type = char8;
        using size_type = i64;
        using difference_type = isize;
        using allocator_type = atl::Polymorphic_Allocator;
        using byte_iterator = char8*;
        using byte_const_iterator = char8*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        // from_utf32
        // Construct String from UTF-32 encoded string.
        //
        // length is the number of bytes to be used to construct String. If length is -1,
        // the function will convert all characters up until and including the null-terminator.
        //
        [[nodiscard]] static String from_utf32(char32 const* string, i64 length = -1);

        // from_utf16
        // Construct String from UTF-16 encoded string.
        //
        // length is the number of bytes to be used to construct String. If length is -1,
        // the function will convert all characters up until and including the null-terminator.
        //
        [[nodiscard]] static String from_utf16(char16 const* string, i64 length = -1);

    public:
        String();
        explicit String(allocator_type const&);
        // Reserve space to fit a string of length n and null-terminator.
        String(Reserve_Tag, size_type n);
        // Reserve space to fit a string of length n and null-terminator.
        String(Reserve_Tag, size_type n, allocator_type const&);
        // Constructs String from null-terminated UTF-8 string
        // TODO: Consider making explicit
        String(value_type const*);
        // Constructs String from null-terminated UTF-8 string
        String(value_type const*, allocator_type const&);
        // Constructs String from null-terminated UTF-8 string
        String(value_type const*, size_type);
        // Constructs String from null-terminated UTF-8 string
        String(value_type const*, size_type, allocator_type const&);
        explicit String(String_View);
        explicit String(String_View, allocator_type const&);
        // Does not copy the allocator
        String(String const&);
        String(String const&, allocator_type const&);
        String(String&&) noexcept;
        String(String&&, allocator_type const&);
        ~String();

        // Does not copy the allocator
        String& operator=(String const&);
        String& operator=(String&&) noexcept;
        String& operator=(String_View);
        String& operator=(value_type const*);

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

        // Capacity of the string in bytes.
        [[nodiscard]] size_type capacity() const;
        // Size of the string in bytes.
        [[nodiscard]] size_type size_bytes() const;
        // Counts the number of Unicode code points.
        // This is a linear-time operation.
        [[nodiscard]] size_type size_utf8() const;

        // reserve
        // Allocates at least requested_capacity + 1 (for null-terminator) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void reserve(size_type requested_capacity);

        // reserve_exact
        // Allocates exactly requested_capacity (no null-terminator) bytes of storage.
        // Does nothing if requested_capacity is less than capacity().
        //
        void reserve_exact(size_type requested_capacity);

        // force_size
        // Changes the size of the string to n. Useful in situations when the user
        // writes to the string via external means.
        //
        void force_size(size_type n);

        void clear();
        void append(char8);
        void append(char32);
        void append(String_View);
        // template <typename Input_Iterator>
        // iterator insert(const_iterator pos, Input_Iterator first, Input_Iterator last);

        [[nodiscard]] value_type* data();
        [[nodiscard]] value_type const* data() const;
        [[nodiscard]] allocator_type& get_allocator();
        [[nodiscard]] allocator_type const& get_allocator() const;

        friend void swap(atl::String&, atl::String&);

    private:
        allocator_type _allocator;
        value_type* _data = nullptr;
        size_type _capacity = 64;
        size_type _size = 0;

        void ensure_capacity(size_type requested_capacity);
        void ensure_capacity_exact(size_type requested_capacity);
    };

    String& operator+=(String&, char8);
    String& operator+=(String&, char32);
    String& operator+=(String&, String_View);

    [[nodiscard]] atl::String operator+(atl::String const& lhs, atl::String const& rhs);
    [[nodiscard]] atl::String operator+(atl::String_View, atl::String const&);
    [[nodiscard]] atl::String operator+(atl::String const&, atl::String_View);
    [[nodiscard]] atl::String operator+(char8 const*, atl::String const&);
    [[nodiscard]] atl::String operator+(atl::String const&, char8 const*);

    // compare
    // Compares two strings.
    // CURRENTLY COMPARES ONLY ASCII
    // Returns: -1 if lhs < rhs, 0 if lhs == rhs and 1 if lhs > rhs.
    //
    // TODO: Consider creating a dedicated class for the return type
    //
    [[nodiscard]] i32 compare(String const& lhs, String const& rhs);

    String to_string(i32);
    String to_string(u32);
    String to_string(i64);
    String to_string(u64);
    String to_string(f32);
    String to_string(f64);
    String to_string(void*);

    // TODO: Implement in terms of String_View.
    f32 str_to_f32(atl::String const& string);

    template<>
    struct Default_Hash<atl::String> {
        using transparent = void;

        [[nodiscard]] constexpr u64 operator()(atl::String_View const v) const {
            return atl::hash(v);
        }
    };

    template<>
    struct Equal_Compare<atl::String> {
        using transparent = void;

        [[nodiscard]] constexpr bool operator()(atl::String_View const lhs, atl::String_View const rhs) const {
            return lhs == rhs;
        }
    };
} // namespace anton_engine::atl

namespace anton_engine {
    namespace serialization {
        class Binary_Output_Archive;
        class Binary_Input_Archive;
    } // namespace serialization

    void serialize(serialization::Binary_Output_Archive&, atl::String const&);
    void deserialize(serialization::Binary_Input_Archive&, atl::String&);
} // namespace anton_engine

#endif // !CORE_ATL_STRING_HPP_INCLUDE
