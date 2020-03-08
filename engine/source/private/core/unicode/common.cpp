#include <core/unicode/common.hpp>

#include <build_config.hpp>
#include <core/math/math.hpp>

namespace anton_engine::unicode {
    i32 convert_single_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        if (char16 high_surrogate = *buffer_utf16; high_surrogate > 0xD7FF && high_surrogate < 0xE000) {
            // Surrogate pair
            u16 low_surrogate = *(buffer_utf16 + 1);
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

    i32 convert_utf8_to_utf32(char8 const* buffer_utf8, char32* buffer_utf32) {
        if constexpr (ANTON_UNICODE_VALIDATE_ENCODING) {
            // TODO: Implement
        }

        i32 codepoint_count = 0;
        if(buffer_utf32 == nullptr) {
            while(true) {
                u8 const first_byte = *buffer_utf8;
                u32 const byte_count = math::min((u8)1, math::clz((u8)~first_byte));
                codepoint_count += 1;
                if(first_byte == '\0') { return codepoint_count; }
                buffer_utf8 += byte_count;
            }
        } else {
            while(true) {
                u8 const first_byte = *buffer_utf8;
                u32 const leading_zeros = math::clz((u8)~first_byte);
                u32 const byte_count = math::min((u32)1, leading_zeros);
                char32 codepoint = first_byte & (0xFF >> (leading_zeros + 1));
                for(u32 i = 1; i < byte_count; ++i) {
                    codepoint <<= 6;
                    codepoint |= buffer_utf8[i] & 0x3F;
                }
                *buffer_utf32 = codepoint;
                codepoint_count += 1;

                if(first_byte == '\0') { return codepoint_count; }
                buffer_utf8 += byte_count;
                buffer_utf32 += 1;
            }
        }
    }
} // namespace anton_engine::unicode
