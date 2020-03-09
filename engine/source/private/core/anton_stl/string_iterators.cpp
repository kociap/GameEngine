#include <core/assert.hpp>
#include <core/stl/detail/string_iterators.hpp>

namespace anton_engine::anton_stl {
    constexpr u8 continuation_byte_signature_mask = 0b11000000;
    constexpr u8 continuation_byte_signature = 0b10000000;
    constexpr u8 continuation_byte_value_mask = 0b00111111;

    constexpr u8 leading_byte_mask = 0b11111000;
    constexpr u8 leading_byte_ascii = 0b01111111;
    constexpr u8 leading_byte_2byte = 0b11011111;
    constexpr u8 leading_byte_3byte = 0b11101111;
    constexpr u8 leading_byte_4byte = 0b11110111;

    UTF8_Char_Iterator::UTF8_Char_Iterator(char8 const* p): data(p) {}

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator++() {
        do {
            ++data;
        } while ((*data & continuation_byte_signature_mask) == continuation_byte_signature);
        return *this;
    }

    UTF8_Char_Iterator UTF8_Char_Iterator::operator++(int) {
        UTF8_Char_Iterator copy = *this;
        ++(*this);
        return copy;
    }

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator--() {
        do {
            --data;
        } while ((*data & continuation_byte_signature_mask) == continuation_byte_signature);
        return *this;
    }

    UTF8_Char_Iterator UTF8_Char_Iterator::operator--(int) {
        UTF8_Char_Iterator copy = *this;
        --(*this);
        return copy;
    }

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator+=(difference_type n) {
        if (n > 0) {
            for (; n != 0; --n) {
                ++(*this);
            }
        } else {
            for (; n != 0; ++n) {
                --(*this);
            }
        }

        return *this;
    }

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator-=(difference_type n) {
        return (*this) += -n;
    }

    UTF8_Char_Iterator::value_type UTF8_Char_Iterator::operator*() const {
        u8 leading_masked = *data & leading_byte_mask;
        ANTON_VERIFY((leading_masked & leading_byte_ascii) == leading_masked || (leading_masked & leading_byte_2byte) == leading_masked ||
                         (leading_masked & leading_byte_3byte) == leading_masked || (leading_masked & leading_byte_4byte) == leading_masked,
                     "Invalid leading UTF-8 byte");

        if ((leading_masked & leading_byte_ascii) == leading_masked) {
            return static_cast<value_type>(data[0]);
        } else if ((leading_masked & leading_byte_2byte) == leading_masked) {
            return (static_cast<value_type>(data[0] & 0b00011111) << 6) | static_cast<value_type>(data[1] & continuation_byte_value_mask);
        } else if ((leading_masked & leading_byte_3byte) == leading_masked) {
            return (static_cast<value_type>(data[0] & 0b00001111) << 12) | (static_cast<value_type>(data[1] & continuation_byte_value_mask) << 6) |
                   static_cast<value_type>(data[2] & continuation_byte_value_mask);
        } else {
            return (static_cast<value_type>(data[0] & 0b00000111) << 18) | (static_cast<value_type>(data[1] & continuation_byte_value_mask) << 12) |
                   (static_cast<value_type>(data[2] & continuation_byte_value_mask) << 6) | static_cast<value_type>(data[3] & continuation_byte_value_mask);
        }
    }

    UTF8_Bytes::UTF8_Bytes(value_type* first, value_type* last): _begin(first), _end(last) {}

    auto UTF8_Bytes::begin() -> iterator {
        return _begin;
    }

    auto UTF8_Bytes::begin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Bytes::cbegin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Bytes::end() -> iterator {
        return _end;
    }

    auto UTF8_Bytes::end() const -> const_iterator {
        return _end;
    }

    auto UTF8_Bytes::cend() const -> const_iterator {
        return _end;
    }

    UTF8_Const_Bytes::UTF8_Const_Bytes(value_type const* first, value_type const* last): _begin(first), _end(last) {}

    auto UTF8_Const_Bytes::begin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Const_Bytes::cbegin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Const_Bytes::end() const -> const_iterator {
        return _end;
    }

    auto UTF8_Const_Bytes::cend() const -> const_iterator {
        return _end;
    }

    UTF8_Chars::UTF8_Chars(value_type const* first, value_type const* last): _begin(first), _end(last) {}

    auto UTF8_Chars::begin() -> iterator {
        return _begin;
    }

    auto UTF8_Chars::begin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Chars::cbegin() const -> const_iterator {
        return _begin;
    }

    auto UTF8_Chars::end() -> iterator {
        return _end;
    }

    auto UTF8_Chars::end() const -> const_iterator {
        return _end;
    }

    auto UTF8_Chars::cend() const -> const_iterator {
        return _end;
    }
} // namespace anton_engine::anton_stl
