#include <anton_stl/string_view.hpp>

namespace anton_stl {
    auto String_View::bytes() const -> UTF8_Const_Bytes {
        return {_begin, _end};
    }

    auto String_View::const_bytes() const -> UTF8_Const_Bytes {
        return {_begin, _end};
    }

    auto String_View::chars() const -> UTF8_Chars {
        return {_begin, _end};
    }

    auto String_View::bytes_begin() const -> byte_const_iterator {
        return _begin;
    }

    auto String_View::bytes_cbegin() const -> byte_const_iterator {
        return _begin;
    }

    auto String_View::bytes_end() const -> byte_const_iterator {
        return _end;
    }

    auto String_View::bytes_cend() const -> byte_const_iterator {
        return _end;
    }

    auto String_View::chars_begin() const -> char_iterator {
        return _begin;
    }

    auto String_View::chars_end() const -> char_iterator {
        return _end;
    }

    bool operator==(String_View const& lhs, String_View const& rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        for (String_View::size_type i = 0; i < lhs.size(); ++i) {
            if (lhs.data()[i] != rhs.data()[i]) { // TODO: .data()[]
                return false;
            }
        }

        return true;
    }
} // namespace anton_stl
