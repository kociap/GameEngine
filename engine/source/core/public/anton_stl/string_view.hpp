#ifndef CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
#define CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE

#include <anton_stl/config.hpp>
#include <anton_stl/detail/string_iterators.hpp>
#include <anton_stl/iterators.hpp>
#include <build_config.hpp>

namespace anton_engine::anton_stl {
    class String_View {
    public:
        using size_type = anton_stl::ssize_t;
        using difference_type = anton_stl::ptrdiff_t;
        using value_type = char;
        using byte_iterator = char*;
        using byte_const_iterator = char const*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        /* [[nodiscard]] */ constexpr String_View();
        /* [[nodiscard]] */ constexpr String_View(String_View const&) = default;
        /* [[nodiscard]] */ constexpr String_View(value_type const*, size_type);
        /* [[nodiscard]] */ constexpr String_View(value_type const*);
        String_View& operator=(String_View const&) = default;

        [[nodiscard]] UTF8_Const_Bytes bytes() const;
        [[nodiscard]] UTF8_Const_Bytes const_bytes() const;
        [[nodiscard]] UTF8_Chars chars() const;

        [[nodiscard]] byte_const_iterator bytes_begin() const;
        [[nodiscard]] byte_const_iterator bytes_cbegin() const;

        [[nodiscard]] byte_const_iterator bytes_end() const;
        [[nodiscard]] byte_const_iterator bytes_cend() const;

        // Always const
        [[nodiscard]] char_iterator chars_begin() const;
        // Always const
        [[nodiscard]] char_iterator chars_end() const;

        // Size of the string in bytes
        [[nodiscard]] constexpr size_type size_bytes() const;

        [[nodiscard]] constexpr value_type const* data() const;

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    // Compares bytes
    [[nodiscard]] bool operator==(String_View const&, String_View const&);

    // Compares bytes
    [[nodiscard]] inline bool operator!=(String_View const& lhs, String_View const& rhs) {
        return !(lhs == rhs);
    }

    inline constexpr String_View::String_View(): _begin(nullptr), _end(nullptr) {}

    inline constexpr String_View::String_View(value_type const* s, size_type n): _begin(s), _end(s + n) {
        // clang-format off
        #if ANTON_STRING_VIEW_VERIFY_ENCODING
                // TODO: Implement
        #endif
        // clang-format on
    }

    inline constexpr String_View::String_View(value_type const* s): _begin(s), _end(s) {
        if (_begin != nullptr) {
            while (*_end)
                ++_end;
        }

        // clang-format off
        #if ANTON_STRING_VIEW_VERIFY_ENCODING
            // TODO: Implement
        #endif
        // clang-format on
    }

    inline constexpr auto String_View::size_bytes() const -> size_type {
        return _end - _begin;
    }

    inline constexpr auto String_View::data() const -> value_type const* {
        return _begin;
    }
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
