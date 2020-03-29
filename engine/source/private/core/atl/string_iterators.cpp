#include <core/atl/detail/string_iterators.hpp>

#include <core/assert.hpp>
#include <core/exception.hpp>
#include <core/math/math.hpp>

namespace anton_engine::atl {
    UTF8_Char_Iterator::UTF8_Char_Iterator(char8 const* p, i64 offset): _data(p), _offset(offset) {}

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator++() {
        if(_offset >= 0) {
            u8 const first_byte = *_data;
            u8 const leading_zeros = math::clz((u8)~first_byte);
            u32 const byte_count = math::max((u8)1, leading_zeros);
            _data += byte_count;
            _offset += byte_count;
            return *this;
        } else {
            _offset += 1;
            _data += 1;
            return *this;
        }
    }

    UTF8_Char_Iterator UTF8_Char_Iterator::operator++(int) {
        UTF8_Char_Iterator copy = *this;
        ++(*this);
        return copy;
    }

    UTF8_Char_Iterator& UTF8_Char_Iterator::operator--() {
        if(_offset != 0) {
            constexpr u8 data_byte_mask = 0xC0;
            constexpr u8 data_byte = 0x80;
            do {
                _data -= 1;
                _offset -= 1;
            } while ((*_data & data_byte_mask) == data_byte);
            return *this;
        } else {
            _offset -= 1;
            _data -= 1;
            return *this;
        }
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
        // TODO: Use unicode library instead.
        u8 const leading_zeros = math::clz((u8)~_data[0]);
        u32 const byte_count = math::max((u8)1, leading_zeros);
        switch(byte_count) {
            case 1: 
                return _data[0];
            case 2:
                return (_data[0] & 0x1F << 6) | (_data[1] & 0x3F);
            case 3:
                return (_data[0] & 0x1F << 12) | (_data[1] & 0x3F << 6);
            case 4:
                return (_data[0] & 0x1F << 18) | (_data[1] & 0x3F << 12) | (_data[2] & 0x3F << 6) | (_data[3] & 0x3F);
            default:
                throw Exception(u8"Unsupported UTF-8 codepoint size.");
        }
    }

    char8 const* UTF8_Char_Iterator::get_underlying_pointer() const {
        return _data;
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
        return iterator{_begin, 0};
    }

    auto UTF8_Chars::begin() const -> const_iterator {
        return const_iterator{_begin, 0};
    }

    auto UTF8_Chars::cbegin() const -> const_iterator {
        return const_iterator{_begin, 0};
    }

    auto UTF8_Chars::end() -> iterator {
        return iterator{_end, _end - _begin};
    }

    auto UTF8_Chars::end() const -> const_iterator {
        return const_iterator{_end, _end - _begin};
    }

    auto UTF8_Chars::cend() const -> const_iterator {
        return const_iterator{_end, _end - _begin};
    }
} // namespace anton_engine::atl
