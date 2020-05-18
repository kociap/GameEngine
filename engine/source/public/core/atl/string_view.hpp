#ifndef CORE_ATL_STRING_VIEW_HPP_INCLUDE
#define CORE_ATL_STRING_VIEW_HPP_INCLUDE

#include <build_config.hpp>
#include <core/atl/detail/string_common.hpp>
#include <core/atl/iterators.hpp>
#include <core/atl/string_utils.hpp>
#include <core/atl/utility.hpp>
#include <core/hashing/murmurhash2.hpp>
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
        constexpr String_View(): _begin(nullptr), _end(nullptr) {}

        constexpr String_View(String_View const&) = default;

        constexpr String_View(value_type const* str): _begin(str), _end(str) {
            if(_begin != nullptr) {
                while(*_end)
                    ++_end;
            }

            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String_View(value_type const* str, size_type size): _begin(str), _end(str + size) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        constexpr String_View(value_type const* first, value_type const* last): _begin(first), _end(last) {
            if constexpr(ANTON_STRING_VIEW_VERIFY_ENCODING) {
                // TODO: Implement
            }
        }

        String_View(char_iterator first, char_iterator last): _begin(first.get_underlying_pointer()), _end(last.get_underlying_pointer()) {}

        constexpr String_View& operator=(String_View const&) = default;

        // TODO: proxies are non-constexpr
        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes bytes() const {
            return {_begin, _end};
        }

        [[nodiscard]] /* constexpr */ UTF8_Const_Bytes const_bytes() const {
            return {_begin, _end};
        }

        [[nodiscard]] /* constexpr */ UTF8_Chars chars() const {
            return {_begin, _end};
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_begin() const {
            return _begin;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_cbegin() const {
            return _begin;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_end() const {
            return _end;
        }

        [[nodiscard]] constexpr byte_const_iterator bytes_cend() const {
            return _end;
        }

        // TODO: iterators are non-constexpr
        [[nodiscard]] /* constexpr */ char_iterator chars_begin() const {
            return char_iterator{_begin, 0};
        }

        [[nodiscard]] /* constexpr */ char_iterator chars_end() const {
            return char_iterator{_end, _end - _begin};
        }

        // Size of the string in bytes
        [[nodiscard]] constexpr size_type size_bytes() const {
            return _end - _begin;
        }

        [[nodiscard]] constexpr value_type const* data() const {
            return _begin;
        }

        friend constexpr void swap(String_View& sv1, String_View& sv2) {
            atl::swap(sv1._begin, sv2._begin);
            atl::swap(sv1._end, sv2._end);
        }

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    // Compares bytes
    [[nodiscard]] constexpr bool operator==(String_View const& lhs, String_View const& rhs) {
        if(lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data(), lhs.size_bytes());
    }

    // Compares bytes
    [[nodiscard]] constexpr bool operator!=(String_View const& lhs, String_View const& rhs) {
        return !(lhs == rhs);
    }

    constexpr u64 hash(String_View const view) {
        return murmurhash2_64(view.bytes_begin(), view.size_bytes());
    }

    constexpr i64 find_substring(atl::String_View const string, atl::String_View const substr) {
        // Bruteforce
        char8 const* const string_data = string.data();
        char8 const* const substr_data = substr.data();
        for(i64 i = 0, end = string.size_bytes() - substr.size_bytes(); i < end; ++i) {
            bool equal = true;
            for(i64 j = i, k = 0; k < substr.size_bytes(); ++j, ++k) {
                equal &= string_data[j] == substr_data[k];
            }

            if(equal) {
                return i;
            }
        }
        return npos;
    }

    // str_to_integer
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Accepts both minus and plus signs only if T is signed. Otherwise accepts neither.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    template<typename T>
    [[nodiscard]] constexpr T str_to_integer(atl::String_View const string, u64 const base = 10) {
        // Convert ['0', '9'] ['A', 'Z'] ['a', 'z'] to [0, 35], everything else to 255.
        constexpr u8 byte_to_digit[] = {
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
            255, 255, 255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
            32,  33,  34,  35,  255, 255, 255, 255, 255, 255, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,
            29,  30,  31,  32,  33,  34,  35,  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

        char8 const* first = string.bytes_begin();
        bool negative = false;
        if constexpr(is_signed<T>) {
            if(*first == '-' || *first == '+') {
                negative = (*first == '-');
                ++first;
            }
        }

        T number = 0;
        char8 const* last = string.bytes_end();
        for(; first != last; ++first) {
            u8 const mapped = byte_to_digit[static_cast<u8>(*first)];
            if(mapped <= base) {
                number = number * base + mapped;
            } else {
                break;
            }
        }

        if constexpr(is_signed<T>) {
            if(negative) {
                number = -number;
            }
        }

        return number;
    }

    // str_to_i64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Accepts both minus and plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] constexpr i64 str_to_i64(atl::String_View const string, u64 const base = 10) {
        return str_to_integer<i64>(string, base);
    }

    // str_to_i64
    // Expects a string containing a number in base [2, 36].
    // There must be no whitespace in the string.
    // Does not accepts either minus or plus signs.
    // Does not recognize 0b and 0x prefixes.
    // Does not guard against overflow.
    // If the string is not valid, the return value is unspecified.
    //
    [[nodiscard]] constexpr u64 str_to_u64(atl::String_View const string, u64 const base = 10) {
        return str_to_integer<u64>(string, base);
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_STRING_VIEW_HPP_INCLUDE
