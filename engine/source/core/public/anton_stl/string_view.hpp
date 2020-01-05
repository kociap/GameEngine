#ifndef CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
#define CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE

#include <anton_int.hpp>
#include <anton_stl/detail/string_iterators.hpp>
#include <anton_stl/iterators.hpp>
#include <anton_stl/string_utils.hpp>
#include <anton_stl/utility.hpp>
#include <build_config.hpp>
#include <hashing/murmurhash2.hpp>

#include <functional> // std::hash

namespace anton_engine::anton_stl {
    class String_View {
    public:
        using size_type = i64;
        using difference_type = isize;
        using value_type = char;
        using byte_iterator = char*;
        using byte_const_iterator = char const*;
        using char_iterator = UTF8_Char_Iterator;

    public:
        /* [[nodiscard]] */ constexpr String_View();
        /* [[nodiscard]] */ constexpr String_View(String_View const&) = default;
        /* [[nodiscard]] */ constexpr String_View(value_type const*, size_type);
        /* [[nodiscard]] */ constexpr String_View(value_type const*);
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
    [[nodiscard]] constexpr bool operator==(String_View const& lhs, String_View const& rhs) {
        if (lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data());
    }

    // Compares bytes
    [[nodiscard]] constexpr bool operator!=(String_View const& lhs, String_View const& rhs) {
        return !(lhs == rhs);
    }

    inline u64 hash(String_View const view) {
        // Seeded with a randomly picked prime number. No idea how that affects the performance or collision frequency.
        // TODO: Do my research on seeding the hash function.
        return anton_engine::murmurhash2(view.bytes_begin(), view.size_bytes(), 547391837);
    }

} // namespace anton_engine::anton_stl

namespace std {
    template <>
    struct hash<anton_engine::anton_stl::String_View> {
        uint64_t operator()(anton_engine::anton_stl::String_View const view) const {
            return anton_engine::anton_stl::hash(view);
        }
    };
} // namespace std

namespace anton_engine::anton_stl {

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
        return _begin;
    }

    inline /* constexpr */ auto String_View::chars_end() const -> char_iterator {
        return _end;
    }

    constexpr auto String_View::size_bytes() const -> size_type {
        return _end - _begin;
    }

    constexpr auto String_View::data() const -> value_type const* {
        return _begin;
    }

    constexpr void swap(String_View& sv1, String_View& sv2) {
        anton_stl::swap(sv1._begin, sv2._begin);
        anton_stl::swap(sv1._end, sv2._end);
    }
} // namespace anton_engine::anton_stl

#endif // !CORE_ANTON_STL_STRING_VIEW_HPP_INCLUDE
