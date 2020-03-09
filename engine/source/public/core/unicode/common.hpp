#ifndef CORE_UNICODE_COMMON_HPP_INCLUDE
#define CORE_UNICODE_COMMON_HPP_INCLUDE

#include <core/types.hpp>

namespace anton_engine::unicode {
    // convert_codepoint_utf16_to_utf8
    // Converts a single UTF-16 codepoint contained in buffer_utf16 to UTF-8 codepoint
    // and writes it to buffer_utf8.
    // If the UTF-16 encoded codepoint is only 2 bytes, this function does not
    // access the second char16.
    //
    // Returns:
    // The number of UTF-8 bytes written.
    //
    // Exceptions:
    // If buffer_utf16 does not contain valid UTF-16, then the function raises
    // Exception if ANTON_UNICODE_VALIDATE_ENCODING is defined.
    // Otherwise the behaviour is undefined.
    //
    i32 convert_codepoint_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8);

    // convert_utf16_to_utf8
    // Converts a UTF-16 encoded string contained in buffer_utf16 to a UTF-8 encoded
    // string and writes it to buffer_utf8.
    // If buffer_utf8 is nullptr, this function calculates the size in bytes required
    // to fit converted string in buffer_utf8.
    //
    // Returns:
    // The number of UTF-8 bytes written or the size of buffer_utf8 in bytes
    // required to store the converted string if buffer_utf8 is nullptr.
    //
    // Exceptions:
    // If buffer_utf16 does not contain valid UTF-16, then the function raises
    // Exception if ANTON_UNICODE_VALIDATE_ENCODING is defined.
    // Otherwise the behaviour is undefined.
    //
    i32 convert_utf16_to_utf8(char16 const* buffer_utf16, char8* buffer_utf8);

    // convert_utf8_to_utf32
    // Converts a UTF-8 encoded string contained in buffer_utf8 to a UTF-32 encoded
    // string and writes it to buffer_utf32.
    // If buffer_utf32 is nullptr, this function calculates the size in bytes required
    // to fit converted string in buffer_utf32.
    //
    // Returns:
    // The number of UTF-32 bytes written or the size of buffer_utf32 in bytes
    // required to store the converted string if buffer_utf32 is nullptr.
    //
    // Exceptions:
    // If buffer_utf8 does not contain valid UTF-8, then the function raises
    // Exception if ANTON_UNICODE_VALIDATE_ENCODING is defined.
    // Otherwise the behaviour is undefined.
    //
    i32 convert_utf8_to_utf32(char8 const* buffer_utf8, char32* buffer_utf32);
} // namespace anton_engine::unicode

#endif // !CORE_UNICODE_COMMON_HPP_INCLUDE
