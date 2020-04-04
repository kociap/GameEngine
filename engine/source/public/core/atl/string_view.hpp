#ifndef CORE_ATL_STRING_VIEW_HPP_INCLUDE
#define CORE_ATL_STRING_VIEW_HPP_INCLUDE

#include <build_config.hpp>
#include <core/hashing/murmurhash2.hpp>
#include <core/atl/detail/string_common.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/string_utils.hpp>
#include <core/atl/utility.hpp>
#include <core/types.hpp>

namespace anton_engine::atl {
    class String_View {
    public:
        using size_type = i64;
        using difference_type = isize;
        using value_type = char8;
        using byte_iterator = char8*;
        using byte_const_iterator = char8 const*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        /* [[nodiscard]] */ constexpr String_View();
        /* [[nodiscard]] */ constexpr String_View(String_View const&) = default;
        /* [[nodiscard]] */ constexpr String_View(value_type const* str, size_type size);
        /* [[nodiscard]] */ constexpr String_View(value_type const* str);
        String_View(char_iterator first, char_iterator last);
        constexpr String_View& operator=(String_View const&) = default;

        // TODO: proxies are non-constexpr
        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes bytes() const;
        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes const_bytes() const;
        [[nodiscard]] /* constexpr */ UTF8_Chars chars() const;

        [[nodiscard]] constexpr byte_const_iterator bytes_begin() const;
        [[nodiscard]] constexpr byte_const_iterator bytes_cbegin() const;
        [[nodiscard]] constexpr byte_const_iterator bytes_end() const;
        [[nodiscard]] constexpr byte_const_iterator bytes_cend() const;

        // TODO: iterators are non-constexpr
        [[nodiscard]] /* constexpr */ char_iterator chars_begin() const;
        [[nodiscard]] /* constexpr */ char_iterator chars_end() const;

        // Size of the string in bytes
        [[nodiscard]] constexpr size_type size_bytes() const;

        [[nodiscard]] constexpr value_type const* data() const;

        friend constexpr void swap(String_View&, String_View&);

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    // Compares bytes
    [[nodiscard]] inline bool operator==(String_View const& lhs, String_View const& rhs) {
        if (lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data());
    }

    // Compares bytes
    [[nodiscard]] inline bool operator!=(String_View const& lhs, String_View const& rhs) {
        return !(lhs == rhs);
    }

    constexpr u64 hash(String_View const view) {
        // Seeded with a randomly picked prime number. No idea how that affects the performance or collision frequency.
        // TODO: Do my research on seeding the hash function.
        return anton_engine::murmurhash2_64(view.bytes_begin(), view.size_bytes(), 547391837);
    }
} // namespace anton_engine::atl

namespace std {
    template <typename T>
    struct hash;

    template <>
    struct hash<anton_engine::atl::String_View> {
        anton_engine::u64 operator()(anton_engine::atl::String_View const view) const {
            return anton_engine::atl::hash(view);
        }
    };
} // namespace std

namespace anton_engine::atl {
    constexpr String_View::String_View(): _begin(nullptr), _end(nullptr) {}

    constexpr String_View::String_View(value_type const* s, size_type n): _begin(s), _end(s + n) {
        if constexpr (ANTON_STRING_VIEW_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    constexpr String_View::String_View(value_type const* s): _begin(s), _end(s) {
        if (_begin != nullptr) {
            while (*_end)
                ++_end;
        }

        if constexpr (ANTON_STRING_VIEW_VERIFY_ENCODING) {
            // TODO: Implement
        }
    }

    inline String_View::String_View(char_iterator first, char_iterator last): 
        _begin(first.get_underlying_pointer()), _end(last.get_underlying_pointer()) {}

    inline /* constexpr */ auto String_View::bytes() const -> UTF8_Const_Bytes {
        return {_begin, _end};
    }

    inline /* constexpr */ auto String_View::const_bytes() const -> UTF8_Const_Bytes {
        return {_begin, _end};
    }

    inline /* constexpr */ auto String_View::chars() const -> UTF8_Chars {
        return {_begin, _end};
    }

    constexpr auto String_View::bytes_begin() const -> byte_const_iterator {
        return _begin;
    }

    constexpr auto String_View::bytes_cbegin() const -> byte_const_iterator {
        return _begin;
    }

    constexpr auto String_View::bytes_end() const -> byte_const_iterator {
        return _end;
    }

    constexpr auto String_View::bytes_cend() const -> byte_const_iterator {
        return _end;
    }

    inline /* constexpr */ auto String_View::chars_begin() const -> char_iterator {
        return char_iterator{_begin, 0};
    }

    inline /* constexpr */ auto String_View::chars_end() const -> char_iterator {
        return char_iterator{_end, _end - _begin};
    }

    constexpr auto String_View::size_bytes() const -> size_type {
        return _end - _begin;
    }

    constexpr auto String_View::data() const -> value_type const* {
        return _begin;
    }

    constexpr void swap(String_View& sv1, String_View& sv2) {
        atl::swap(sv1._begin, sv2._begin);
        atl::swap(sv1._end, sv2._end);
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_STRING_VIEW_HPP_INCLUDE
