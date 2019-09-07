#ifndef CORE_UNICODE_COMMON_HPP_INCLUDE
#define CORE_UNICODE_COMMON_HPP_INCLUDE

#include <cstdint>

namespace unicode {
    // convert_utf16_to_utf8
    // Converts a UTF-16 codepoint contained in buffer_utf16 to UTF-8 codepoint
    // and writes it to buffer_utf8.
    // If the UTF-16 encoded codepoint is only 2 bytes, this function does not
    // access the second char16.
    //
    // Returns:
    // The number of UTF-8 bytes written.
    //
    // Exceptions:
    // If buffer_utf16 does not contain valid UTF-16, then the function raises
    // std::invalid_argument if ANTON_UNICODE_VALIDATE_ENCODING is defined.
    // Otherwise the behaviour is undefined.
    //
    int32_t convert_utf16_to_utf8(char16_t const* buffer_utf16, char* buffer_utf8);
} // namespace unicode

#endif // !CORE_UNICODE_COMMON_HPP_INCLUDE
