#include <anton_stl/string.hpp>

#include <anton_stl/detail/utility_common.hpp>
#include <anton_stl/memory.hpp>
#include <anton_stl/string_utils.hpp>
#include <anton_stl/type_traits.hpp>
#include <build_config.hpp>
#include <math/math.hpp>
#include <serialization/archives/binary.hpp>
#include <serialization/serialization.hpp>
#include <unicode/common.hpp>

#include <cstdint>  // uintptr_t
#include <string.h> // memset

namespace anton_stl {
    String String::from_utf16(char16_t const* str) {
        char codepoint_utf8[5] = {};
        String string;
        while (*str != char16_t(0)) {
            int32_t bytes_written = unicode::convert_utf16_to_utf8(str, codepoint_utf8);
            string.append(String_View(codepoint_utf8, bytes_written));
            str += (bytes_written < 4 ? 1 : 2);
        }
        return string;
    }

    String::String(): String(allocator_type()) {}

    String::String(allocator_type const& allocator): _allocator(allocator) {
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data, 0, _capacity);
    }

    String::String(Reserve_Tag, size_type n): String(anton_stl::reserve, n, allocator_type()) {}

    String::String(Reserve_Tag, size_type n, allocator_type const&) {
        _capacity = math::max(_capacity - 1, n) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data, 0, _capacity);
    }

    String::String(value_type const* str): String(str, allocator_type()) {}

    String::String(value_type const* cstr, allocator_type const& allocator): _allocator(allocator) {
        _size = strlen(cstr);
        if constexpr (ANTON_STRING_VERIFY_ENCODING) {
            // TODO: Implement
        }
        _capacity = math::max(_capacity - 1, _size) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        anton_stl::copy(cstr, cstr + _size, _data);
    }

    String::String(value_type const* cstr, size_type n): String(cstr, n, allocator_type()) {}

    String::String(value_type const* cstr, size_type n, allocator_type const& allocator): _allocator(allocator) {
        _size = n;
        _capacity = math::max(_capacity - 1, n) + 1;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        anton_stl::copy(cstr, cstr + n, _data);
    }

    String::String(String const& other): String(other, allocator_type()) {}

    String::String(String const& other, allocator_type const& allocator): _allocator(allocator) {
        _size = other._size;
        _capacity = other._capacity;
        _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        memset(_data + _size, 0, _capacity - _size);
        anton_stl::copy(other._data, other._data + other._size, _data);
    }

    String::String(String&& other) noexcept: _allocator(anton_stl::move(other._allocator)), _data(other._data), _size(other._size) {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    String::String(String&& other, allocator_type const& allocator): _allocator(allocator) {
        if (_allocator == other._allocator) {
            _data = other._data;
            other._data = nullptr;
            _capacity = other._capacity;
            other._capacity = 0;
            _size = other._size;
            other._size = 0;
        } else {
            _size = other._size;
            _capacity = other._capacity;
            _data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
            memset(_data + _size, 0, _capacity - _size);
            anton_stl::move(other._data, other._data + other._size, _data);
        }
    }

    String::~String() {
        if (_data != nullptr) {
            _allocator.deallocate(_data, _capacity, alignof(value_type));
        }
    }

    String& String::operator=(String const& other) {
        value_type* new_data = reinterpret_cast<value_type*>(_allocator.allocate(_capacity, alignof(value_type)));
        _allocator.deallocate(_data, _capacity, alignof(value_type));
        _data = new_data;
        _size = other._size;
        _capacity = other._capacity;
        memset(_data + _size, 0, _capacity - _size);
        anton_stl::copy(other._data, other._data + other._size, _data);
        return *this;
    }

    String& String::operator=(String&& other) noexcept {
        swap(*this, other);
        return *this;
    }

    // Implicit conversion operator
    String::operator String_View() const {
        return {_data, _size};
    }

    auto String::bytes() -> UTF8_Bytes {
        return {_data, _data + _size};
    }

    auto String::bytes() const -> UTF8_Const_Bytes {
        return {_data, _data + _size};
    }

    auto String::const_bytes() const -> UTF8_Const_Bytes {
        return {_data, _data + _size};
    }

    auto String::chars() const -> UTF8_Chars {
        return {_data, _data + _size};
    }

    auto String::bytes_begin() -> byte_iterator {
        return _data;
    }

    auto String::bytes_begin() const -> byte_const_iterator {
        return _data;
    }

    auto String::bytes_cbegin() const -> byte_const_iterator {
        return _data;
    }

    auto String::bytes_end() -> byte_iterator {
        return _data + _size;
    }

    auto String::bytes_end() const -> byte_const_iterator {
        return _data + _size;
    }

    auto String::bytes_cend() const -> byte_const_iterator {
        return _data + _size;
    }

    auto String::chars_begin() const -> char_iterator {
        return _data;
    }

    auto String::chars_end() const -> char_iterator {
        return _data + _size;
    }

    auto String::capacity() const -> size_type {
        return _capacity;
    }

    auto String::size_bytes() const -> size_type {
        return _size;
    }

    auto String::size_utf8() const -> size_type {
        return chars_end() - chars_begin();
    }

    void String::reserve(size_type n) {
        ensure_capacity(n);
    }

    void String::reserve_exact(size_type n) {
        ensure_capacity_exact(n);
    }

    void String::force_size(size_type n) {
        _size = n;
    }

    void String::clear() {
        memset(_data, 0, _size);
        _size = 0;
    }

    void String::append(String_View str) {
        ensure_capacity(_size + str.size_bytes() + 1);
        anton_stl::copy(str.bytes_begin(), str.bytes_end(), _data + _size);
        _size = _size + str.size_bytes();
    }

    auto String::data() -> value_type* {
        return _data;
    }

    auto String::data() const -> value_type const* {
        return _data;
    }

    String::allocator_type& String::get_allocator() {
        return _allocator;
    }

    String::allocator_type const& String::get_allocator() const {
        return _allocator;
    }

    void String::ensure_capacity(size_type requested_capacity) {
        if (requested_capacity > _capacity) {
            size_type new_capacity = _capacity;
            while (new_capacity < requested_capacity) {
                new_capacity *= 2;
            }

            value_type* new_data = static_cast<value_type*>(_allocator.allocate(new_capacity, alignof(value_type)));
            memset(new_data + _size, 0, new_capacity - _size);
            anton_stl::move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = new_capacity;
        }
    }

    void String::ensure_capacity_exact(size_type requested_capacity) {
        if (requested_capacity > _capacity) {
            value_type* new_data = static_cast<value_type*>(_allocator.allocate(requested_capacity, alignof(value_type)));
            memset(new_data + _size, 0, requested_capacity - _size);
            anton_stl::move(_data, _data + _size, new_data);
            _allocator.deallocate(_data, _capacity, alignof(value_type));
            _data = new_data;
            _capacity = requested_capacity;
        }
    }

    void swap(anton_stl::String& str1, anton_stl::String& str2) {
        anton_stl::swap(str1._allocator, str2._allocator);
        anton_stl::swap(str1._data, str2._data);
        anton_stl::swap(str1._capacity, str2._capacity);
        anton_stl::swap(str1._size, str2._size);
    }

    bool operator==(String const& lhs, String const& rhs) {
        if (lhs.size_bytes() != rhs.size_bytes()) {
            return false;
        }

        return compare_equal(lhs.data(), rhs.data());
    }

    String operator+(String const& lhs, String const& rhs) {
        String str(reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(String_View lhs, anton_stl::String const& rhs) {
        String str(reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(String const& lhs, String_View rhs) {
        String str(reserve, lhs.size_bytes() + rhs.size_bytes());
        str.append(lhs);
        str.append(rhs);
        return str;
    }

    String operator+(char const* lhs, anton_stl::String const& rhs) {
        return String_View(lhs) + rhs;
    }

    String operator+(String const& lhs, char const* rhs) {
        return lhs + String_View(rhs);
    }

    anton_stl::String to_string(int value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%d", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(long value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%ld", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(long long value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%lld", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(unsigned int value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%u", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(unsigned long value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%lu", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(unsigned long long value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%llu", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(float value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%f", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(double value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%f", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(long double value) {
        char buffer[50] = {};
        int written_chars = sprintf(buffer, "%Lf", value);
        return {buffer, written_chars};
    }

    anton_stl::String to_string(void* value) {
        char buffer[50] = {};
        uintptr_t address = reinterpret_cast<uintptr_t>(value);
        int written_chars = sprintf(buffer, "0x%016llx", address);
        return {buffer, written_chars};
    }
} // namespace anton_stl

namespace serialization {
    void serialize(Binary_Output_Archive& out, anton_stl::String const& str) {
        out.write(str.capacity());
        out.write(str.size_bytes());
        out.write_binary(str.data(), str.size_bytes());
    }

    void deserialize(Binary_Input_Archive& in, anton_stl::String& str) {
        anton_stl::String::size_type capacity, size;
        in.read(capacity);
        in.read(size);
        str.clear();
        str.reserve_exact(capacity);
        str.force_size(size);
        in.read_binary(str.data(), size);
    }
} // namespace serialization
