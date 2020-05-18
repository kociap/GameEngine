#ifndef CORE_ATL_STRING_UTILS_HPP_INCLUDE
#define CORE_ATL_STRING_UTILS_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine::atl {
    constexpr bool compare_equal(char8 const* lhs, char8 const* rhs) {
        while(*lhs != u8'\0' && *rhs != u8'\0' && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == u8'\0' && *rhs == u8'\0';
    }

    // compare without care for nulls
    // use this for views
    constexpr bool compare_equal(char8 const* lhs, char8 const* rhs, i64 length) {
        for(i64 i = 0; i < length; ++i) {
            if(*lhs != *rhs)
                return false;
            ++lhs;
            ++rhs;
        }
        return true;
    }

    constexpr bool compare_equal(char16 const* lhs, char16 const* rhs) {
        while(*lhs != u'\0' && *rhs != u'\0' && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == u'\0' && *rhs == u'\0';
    }

    constexpr bool compare_equal(char32 const* lhs, char32 const* rhs) {
        while(*lhs != U'\0' && *rhs != U'\0' && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == U'\0' && *rhs == U'\0';
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_STRING_UTILS_HPP_INCLUDE
