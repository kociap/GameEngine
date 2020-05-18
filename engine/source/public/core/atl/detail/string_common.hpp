#ifndef CORE_ATL_DETAIL_STRING_ITERATORS_HPP_INCLUDE
#define CORE_ATL_DETAIL_STRING_ITERATORS_HPP_INCLUDE

#include <core/atl/iterators.hpp>
#include <core/types.hpp>

// TODO: constexpr

namespace anton_engine::atl {
    constexpr i64 npos = -1;

    class String;
    class String_View;
    class UTF8_Chars;

    [[nodiscard]] bool is_whitespace(char32);

    // UTF8_Char_Iterator
    // Iterates over Unicode code points encoded as UTF-8.
    //
    // Notes:
    // Does not provide operator->
    // Does not provide operator[]
    //
    class UTF8_Char_Iterator {
    public:
        using value_type = u32;
        using difference_type = isize;
        using iterator_category = atl::Random_Access_Iterator_Tag;

        UTF8_Char_Iterator() = delete;
        // offset_from_begin is the offset from beginning of a buffer in bytes.
        UTF8_Char_Iterator(char8 const*, i64 offset_from_begin);
        UTF8_Char_Iterator(UTF8_Char_Iterator const&) = default;
        UTF8_Char_Iterator(UTF8_Char_Iterator&&) = default;
        ~UTF8_Char_Iterator() = default;
        UTF8_Char_Iterator& operator=(UTF8_Char_Iterator const&) = default;
        UTF8_Char_Iterator& operator=(UTF8_Char_Iterator&&) = default;

        // Advances the iterator one code point
        UTF8_Char_Iterator& operator++();
        // Advances the iterator one code point
        UTF8_Char_Iterator operator++(int);
        // Recedes the iterator one code point
        UTF8_Char_Iterator& operator--();
        // Recedes the iterator one code point
        UTF8_Char_Iterator operator--(int);

        // Advances the iterator x code points
        // This operation is linear in n.
        UTF8_Char_Iterator& operator+=(difference_type n);

        // Recedes the iterator x code points
        // This operation is linear in n.
        UTF8_Char_Iterator& operator-=(difference_type n);

        // TODO: No idea how to implement that
        // [[nodiscard]] value_type* operator->() const;

        [[nodiscard]] value_type operator*() const;

        char8 const* get_underlying_pointer() const;

        [[nodiscard]] bool operator==(UTF8_Char_Iterator const& b) const {
            return _data == b._data;
        }

        [[nodiscard]] bool operator<(UTF8_Char_Iterator const& b) const {
            return _data - b._data < 0;
        }

    private:
        char8 const* _data;
        i64 _offset;
    };

    [[nodiscard]] inline bool operator!=(UTF8_Char_Iterator const& a, UTF8_Char_Iterator const& b) {
        return !(a == b);
    }

    [[nodiscard]] inline bool operator>(UTF8_Char_Iterator const& a, UTF8_Char_Iterator const& b) {
        return b < a;
    }

    [[nodiscard]] inline bool operator<=(UTF8_Char_Iterator const& a, UTF8_Char_Iterator const& b) {
        return !(a > b);
    }

    [[nodiscard]] inline bool operator>=(UTF8_Char_Iterator const& a, UTF8_Char_Iterator const& b) {
        return !(a < b);
    }

    // Returns a copy of the iterator advanced by n
    // This operation is linear in n.
    [[nodiscard]] inline UTF8_Char_Iterator operator+(UTF8_Char_Iterator a, UTF8_Char_Iterator::difference_type n) {
        a += n;
        return a;
    }

    // Returns a copy of the iterator advanced by n
    // This operation is linear in n.
    [[nodiscard]] inline UTF8_Char_Iterator operator+(UTF8_Char_Iterator::difference_type n, UTF8_Char_Iterator a) {
        a += n;
        return a;
    }

    // Returns a copy of the iterator receded by n
    // This operation is linear in n.
    [[nodiscard]] inline UTF8_Char_Iterator operator-(UTF8_Char_Iterator a, UTF8_Char_Iterator::difference_type n) {
        a -= n;
        return a;
    }

    // Computes the number of UTF-8 code points between lhs and rhs.
    // This is a linear-time operation.
    [[nodiscard]] inline UTF8_Char_Iterator::difference_type operator-(UTF8_Char_Iterator const& lhs, UTF8_Char_Iterator rhs) {
        UTF8_Char_Iterator::difference_type difference = 0;
        if(lhs < rhs) {
            while(lhs != rhs) {
                --rhs;
                difference -= 1;
            }
        } else {
            while(lhs != rhs) {
                ++rhs;
                difference += 1;
            }
        }
        return difference;
    }

    class UTF8_Bytes {
    public:
        using value_type = char8;
        using iterator = char8*;
        using const_iterator = char8 const*;

        UTF8_Bytes(value_type* first, value_type* last);

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

    private:
        value_type* _begin;
        value_type* _end;
    };

    class UTF8_Const_Bytes {
    public:
        using value_type = char8;
        using iterator = char8 const*;
        using const_iterator = char8 const*;

        UTF8_Const_Bytes(value_type const* first, value_type const* last);

        const_iterator begin() const;
        const_iterator cbegin() const;

        const_iterator end() const;
        const_iterator cend() const;

    private:
        value_type const* _begin;
        value_type const* _end;
    };

    class UTF8_Chars {
    public:
        using value_type = char8;
        using iterator = UTF8_Char_Iterator;
        using const_iterator = UTF8_Char_Iterator;

        UTF8_Chars(value_type const* first, value_type const* last);

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;

        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

    private:
        value_type const* _begin;
        value_type const* _end;
    };
} // namespace anton_engine::atl

#endif // !CORE_ATL_DETAIL_STRING_ITERATORS_HPP_INCLUDE
