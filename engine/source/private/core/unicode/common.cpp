#include <core/unicode/common.hpp>

#include <build_config.hpp>
#include <core/math/math.hpp>

namespace anton_engine::unicode {
    i32 convert_codepoint_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if (char16 high_surrogate = *buffer_utf16; high_surrogate > 0xD7FF && high_surrogate < 0xE000) {
            // Surrogate pair
            u16 low_surrogate = buffer_utf16[1];
            char32 codepoint = (((high_surrogate & 0x3FF) >> 10) | (low_surrogate & 0x3FF)) + 0x10000;
            buffer_utf8[0] = 0xF0 | ((codepoint & 0x1C0000) >> 18);
            buffer_utf8[1] = 0x80 | ((codepoint & 0x3F000) >> 12);
            buffer_utf8[2] = 0x80 | ((codepoint & 0xFC0) >> 6);
            buffer_utf8[3] = 0x80 | (codepoint & 0x3F);
            return 4;
        } else {
            char16 codepoint = *buffer_utf16;
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

    i32 convert_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if (buffer_utf8 == nullptr) {
            i32 bytes = 0;
            while (true) {
                if (char16 high_surrogate = *buffer_utf16; high_surrogate > 0xD7FF && high_surrogate < 0xE000) {
                    bytes += 4;
                    buffer_utf16 += 2;
                } else {
                    char16 codepoint = *buffer_utf16;
                    // If codepoint is less than 0x7FF, it's 2 bytes in UTF-8. If it's less than 0x7F, it's 1 byte in UTF-8.
                    // Otherwise it's 3 bytes.
                    bytes += 3 - (codepoint <= 0x7FF) - (codepoint <= 0x7F);
                    buffer_utf16 += 1;
                    if (codepoint == '\0') {
                        break;
                    }
                }
            }
            return bytes;
        } else {
            i32 bytes_written = 0;
            while (true) {
                if (char16 high_surrogate = *buffer_utf16; high_surrogate > 0xD7FF && high_surrogate < 0xE000) {
                    // Surrogate pair
                    u16 low_surrogate = buffer_utf16[1];
                    char32 codepoint = (((high_surrogate & 0x3FF) >> 10) | (low_surrogate & 0x3FF)) + 0x10000;
                    buffer_utf8[0] = 0xF0 | ((codepoint & 0x1C0000) >> 18);
                    buffer_utf8[1] = 0x80 | ((codepoint & 0x3F000) >> 12);
                    buffer_utf8[2] = 0x80 | ((codepoint & 0xFC0) >> 6);
                    buffer_utf8[3] = 0x80 | (codepoint & 0x3F);
                    buffer_utf8 += 4;
                    bytes_written += 4;
                    buffer_utf16 += 2;
                } else {
                    char16 codepoint = *buffer_utf16;
                    if (codepoint <= 0x7F) {
                        *buffer_utf8 = codepoint;
                        buffer_utf8 += 1;
                        bytes_written += 1;
                        // This is the only case where null terminator could appear.
                        if (codepoint == '\0') {
                            break;
                        }
                    } else if (codepoint <= 0x7FF) {
                        buffer_utf8[0] = 0xC0 | ((codepoint & 0x7C0) >> 6);
                        buffer_utf8[1] = 0x80 | (codepoint & 0x3F);
                        buffer_utf8 += 2;
                        bytes_written += 2;
                    } else {
                        buffer_utf8[0] = 0xE0 | ((codepoint & 0xF000) >> 12);
                        buffer_utf8[1] = 0x80 | ((codepoint & 0xFC0) >> 6);
                        buffer_utf8[2] = 0x80 | (codepoint & 0x3F);
                        buffer_utf8 += 3;
                        bytes_written += 3;
                    }
                    buffer_utf16 += 1;
                }
            }
            return bytes_written;
        }
    }

    i32 convert_utf8_to_utf32(char8 const* buffer_utf8, char32* buffer_utf32) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if (buffer_utf32 == nullptr) {
            i32 codepoint_count = 0;
            while (true) {
                u8 const first_byte = *buffer_utf8;
                u8 const leading_zeros = math::clz((u8)~first_byte);
                u32 const byte_count = math::max((u8)1, leading_zeros);
                codepoint_count += 1;
                if (first_byte == '\0') {
                    return codepoint_count * sizeof(char32);
                }
                buffer_utf8 += byte_count;
            }
        } else {
            i32 codepoint_count = 0;
            while (true) {
                u8 const first_byte = *buffer_utf8;
                u32 const leading_zeros = math::clz((u8)~first_byte);
                u32 const byte_count = math::max((u32)1, leading_zeros);
                char32 codepoint = first_byte & (0xFF >> (leading_zeros + 1));
                for (u32 i = 1; i < byte_count; ++i) {
                    codepoint <<= 6;
                    codepoint |= buffer_utf8[i] & 0x3F;
                }
                *buffer_utf32 = codepoint;
                codepoint_count += 1;

                if (first_byte == '\0') {
                    return codepoint_count * sizeof(char32);
                }
                buffer_utf8 += byte_count;
                buffer_utf32 += 1;
            }
        }
    }
} // namespace anton_engine::unicode
