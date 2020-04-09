#ifndef CORE_ATL_STRING_UTILS_HPP_INCLUDE
#define CORE_ATL_STRING_UTILS_HPP_INCLUDE

namespace anton_engine::atl {
    inline bool compare_equal(char const* lhs, char const* rhs) {
        while (*lhs != '\0' && *rhs != '\0' && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == '\0' && *rhs == '\0';
    }

    inline bool compare_equal(char16_t const* lhs, char16_t const* rhs) {
        while (*lhs != char16_t(0) && *rhs != char16_t(0) && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == char16_t(0) && *rhs == char16_t(0);
    }

    inline bool compare_equal(char32_t const* lhs, char32_t const* rhs) {
        while (*lhs != char32_t(0) && *rhs != char32_t(0) && *lhs == *rhs) {
            ++lhs;
            ++rhs;
        }

        // If we reached the end of both strings, they are equal
        return *lhs == char32_t(0) && *rhs == char32_t(0);
    }
} // namespace anton_engine::atl

#endif // !CORE_ATL_STRING_UTILS_HPP_INCLUDE
