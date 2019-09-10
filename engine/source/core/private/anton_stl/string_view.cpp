#include <anton_stl/string_utils.hpp>
#include <anton_stl/string_view.hpp>

namespace anton_engine::anton_stl {
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
        if (lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data());
    }
} // namespace anton_engine::anton_stl
