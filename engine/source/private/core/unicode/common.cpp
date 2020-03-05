#include <core/unicode/common.hpp>

#include <build_config.hpp>

#include <stdexcept>

namespace anton_engine::unicode {
    int32_t convert_utf16_to_utf8(char16_t const* buffer_utf16, char* buffer_utf8) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if (char16_t high_surrogate = *buffer_utf16; high_surrogate > 0xD7FF && high_surrogate < 0xE000) {
            // Surrogate pair
            uint16_t low_surrogate = *(buffer_utf16 + 1);
            char32_t codepoint = (((high_surrogate & 0x3FF) >> 10) | (low_surrogate & 0x3FF)) + 0x10000;
            buffer_utf8[0] = 0xF0 | ((codepoint & 0x1C0000) >> 18);
            buffer_utf8[1] = 0x80 | ((codepoint & 0x3F000) >> 12);
            buffer_utf8[2] = 0x80 | ((codepoint & 0xFC0) >> 6);
            buffer_utf8[3] = 0x80 | (codepoint & 0x3F);
            return 4;
        } else {
            char16_t codepoint = *buffer_utf16;
            if (codepoint <= 0x7F) {
                *buffer_utf8 = codepoint;
                return 1;
            } else if (codepoint <= 0x7FF) {
                buffer_utf8[0] = 0xC0 | ((codepoint & 0x7C0) >> 6);
                buffer_utf8[1] = 0x80 | (codepoint & 0x3F);
                return 2;
            } else {
                buffer_utf8[0] = 0xE0 | ((codepoint & 0xF000) >> 12);
                buffer_utf8[1] = 0x80 | ((codepoint & 0xFC0) >> 6);
                buffer_utf8[2] = 0x80 | (codepoint & 0x3F);
                return 3;
            }
        }
    }
} // namespace anton_engine::unicode
