#include <core/json.hpp>

#include <core/assert.hpp>
#include <core/atl/flat_hash_map.hpp>
#include <core/atl/string.hpp>
#include <core/atl/vector.hpp>
#include <core/filesystem.hpp>

namespace anton_engine::json {
    struct Null {
        explicit Null() = default;
    };

    constexpr Null null;

    struct _Element {
    public:
        _Element();
        _Element(_Object&& object);
        _Element(_Array&& array);
        _Element(atl::String_View string);
        _Element(atl::String&& string);
        _Element(i64 number);
        _Element(f64 number);
        _Element(bool boolean);
        _Element(Null null);
        _Element(_Element&& element);
        _Element& operator=(_Object&& object);
        _Element& operator=(_Array&& array);
        _Element& operator=(atl::String_View string);
        _Element& operator=(atl::String&& string);
        _Element& operator=(i64 number);
        _Element& operator=(f64 number);
        _Element& operator=(bool boolean);
        _Element& operator=(Null null);
        _Element& operator=(_Element&& element);
        ~_Element();

        [[nodiscard]] Element_Type element_type() const;

        [[nodiscard]] atl::Tuple<Object, bool> as_object() const;
        [[nodiscard]] atl::Tuple<Array, bool> as_array() const;
        [[nodiscard]] atl::Tuple<atl::String_View, bool> as_string() const;
        [[nodiscard]] atl::Tuple<i64, bool> as_i64() const;
        [[nodiscard]] atl::Tuple<f64, bool> as_f64() const;
        [[nodiscard]] atl::Tuple<bool, bool> as_boolean() const;

    private:
        Element_Type _type;
        void* _value;

        void destroy_value();
    };

    struct Object_Member {
        atl::String key;
        _Element value;

        Object_Member() = default;
        Object_Member(atl::String const& s, _Element&& e): key(s), value(atl::move(e)) {}
        Object_Member(atl::String&& s, _Element&& e): key(atl::move(s)), value(atl::move(e)) {}
    };

    struct _Object {
        atl::Flat_Hash_Map<u64, Object_Member> elements;
    };

    Object_Iterator& operator++(Object_Iterator& iterator) {
        auto& i = *(atl::Flat_Hash_Map<u64, Object_Member>::iterator*)&iterator;
        ++i;
        return iterator;
    }

    Key_Value operator*(Object_Iterator& iterator) {
        auto& i = *(atl::Flat_Hash_Map<u64, Object_Member>::iterator*)&iterator;
        Object_Member& member = i->value;
        return {member.key, &member.value};
    }

    bool operator==(Object_Iterator const& lhs, Object_Iterator const& rhs) {
        auto& _lhs = *(atl::Flat_Hash_Map<u64, Object_Member>::iterator*)&lhs;
        auto& _rhs = *(atl::Flat_Hash_Map<u64, Object_Member>::iterator*)&rhs;
        return _lhs == _rhs;
    }

    Object_Iterator begin(Object object) {
        Object_Iterator iterator;
        ::new(&iterator) atl::Flat_Hash_Map<u64, Object_Member>::iterator(object->elements.begin());
        return iterator;
    }

    Object_Iterator end(Object object) {
        Object_Iterator iterator;
        ::new(&iterator) atl::Flat_Hash_Map<u64, Object_Member>::iterator(object->elements.end());
        return iterator;
    }

    struct _Array {
        atl::Vector<_Element> elements;
    };

    Array_Iterator& Array_Iterator::operator++() {
        ++element;
        return *this;
    }

    Array_Iterator& Array_Iterator::operator--() {
        --element;
        return *this;
    }

    Array_Iterator Array_Iterator::operator+(i64 offset) {
        Array_Iterator iterator;
        iterator.element = element + offset;
        return iterator;
    }

    Array_Iterator& Array_Iterator::operator+=(i64 offset) {
        element += offset;
        return *this;
    }

    Array_Iterator Array_Iterator::operator-(i64 offset) {
        Array_Iterator iterator;
        iterator.element = element - offset;
        return iterator;
    }

    Array_Iterator& Array_Iterator::operator-=(i64 offset) {
        element -= offset;
        return *this;
    }

    Element Array_Iterator::operator*() {
        return element;
    }

    bool Array_Iterator::operator==(Array_Iterator const& rhs) const {
        return element == rhs.element;
    }

    bool Array_Iterator::operator!=(Array_Iterator const& rhs) const {
        return element != rhs.element;
    }

    Array_Iterator begin(Array array) {
        return {array->elements.begin()};
    }

    Array_Iterator end(Array array) {
        return {array->elements.end()};
    }

    _Element::_Element(): _type(Element_Type::null), _value(nullptr) {}

    _Element::_Element(_Object&& object) {
        _type = Element_Type::object;
        _value = new _Object(atl::move(object));
    }

    _Element::_Element(_Array&& array) {
        _type = Element_Type::array;
        _value = new _Array(atl::move(array));
    }

    _Element::_Element(atl::String_View string): _type(Element_Type::string), _value(new atl::String(string)) {}

    _Element::_Element(atl::String&& string): _type(Element_Type::string), _value(new atl::String(atl::move(string))) {}

    _Element::_Element(i64 number): _type(Element_Type::number_i64), _value((void*)(*(u64*)&number)) {}

    _Element::_Element(f64 number): _type(Element_Type::number_f64), _value((void*)(*(u64*)&number)) {}

    _Element::_Element(bool boolean): _type(Element_Type::boolean), _value((void*)boolean) {}

    _Element::_Element(Null): _type(Element_Type::null), _value(nullptr) {}

    _Element::_Element(_Element&& element): _type(element._type), _value(element._value) {
        element._type = Element_Type::null;
        element._value = nullptr;
    }

    _Element& _Element::operator=(_Object&& object) {
        destroy_value();
        _type = Element_Type::object;
        _value = new _Object(atl::move(object));
        return *this;
    }

    _Element& _Element::operator=(_Array&& array) {
        destroy_value();
        _type = Element_Type::array;
        _value = new _Array(atl::move(array));
        return *this;
    }

    _Element& _Element::operator=(atl::String_View string) {
        destroy_value();
        _type = Element_Type::string;
        _value = new atl::String(string);
        return *this;
    }

    _Element& _Element::operator=(atl::String&& string) {
        destroy_value();
        _type = Element_Type::string;
        _value = new atl::String(atl::move(string));
        return *this;
    }

    _Element& _Element::operator=(i64 number) {
        destroy_value();
        _type = Element_Type::number_i64;
        _value = (void*)(*(u64*)&number);
        return *this;
    }

    _Element& _Element::operator=(f64 number) {
        destroy_value();
        _type = Element_Type::number_f64;
        _value = (void*)(*(u64*)&number);
        return *this;
    }

    _Element& _Element::operator=(bool boolean) {
        destroy_value();
        _type = Element_Type::boolean;
        _value = (void*)boolean;
        return *this;
    }

    _Element& _Element::operator=(Null) {
        destroy_value();
        _type = Element_Type::null;
        _value = nullptr;
        return *this;
    }

    _Element& _Element::operator=(_Element&& element) {
        atl::swap(_type, element._type);
        atl::swap(_value, element._value);
        return *this;
    }

    _Element::~_Element() {
        destroy_value();
    }

    Element_Type _Element::element_type() const {
        return _type;
    }

    atl::Tuple<Object, bool> _Element::as_object() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::object) {
                return {nullptr, false};
            }
        }
        Object object = (Object)_value;
        return {object, true};
    }

    atl::Tuple<Array, bool> _Element::as_array() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::array) {
                return {nullptr, false};
            }
        }
        Array array = (Array)_value;
        return {array, true};
    }

    atl::Tuple<atl::String_View, bool> _Element::as_string() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::string) {
                return {"", false};
            }
        }
        atl::String const& string = *(atl::String*)_value;
        return {string, true};
    }

    atl::Tuple<i64, bool> _Element::as_i64() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::number_i64) {
                return {0, false};
            }
        }
        i64 const number = *(i64*)&_value;
        return {number, true};
    }

    atl::Tuple<f64, bool> _Element::as_f64() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::number_f64) {
                return {0.0, false};
            }
        }
        f64 const number = *(f64*)&_value;
        return {number, true};
    }

    atl::Tuple<bool, bool> _Element::as_boolean() const {
        if constexpr(ANTON_JSON_VERIFY_TYPE) {
            if(_type != Element_Type::boolean) {
                return {false, false};
            }
        }
        bool const boolean = (bool)_value;
        return {boolean, true};
    }

    void _Element::destroy_value() {
        switch(_type) {
            case Element_Type::object: {
                _Object* object = (_Object*)_value;
                delete object;
            } break;

            case Element_Type::array: {
                _Array* array = (_Array*)_value;
                delete array;
            } break;

            case Element_Type::string: {
                atl::String* string = (atl::String*)_value;
                delete string;
            } break;

            default:
                break;
        }
    }

    Element_Type element_type(Element element) {
        return element->element_type();
    }

    Document::Document(): _root(new _Element(null)) {}

    Document::Document(Element root): _root(root) {}

    Document::Document(Document&& document): _root(document._root) {
        document._root = nullptr;
    }

    Document& Document::operator=(Document&& document) {
        atl::swap(_root, document._root);
        return *this;
    }

    Document::~Document() {
        delete _root;
    }

    Element Document::get_root_element() const {
        return _root;
    }

    struct Lexer_State {
        atl::UTF8_Char_Iterator current;
    };

    struct Lexer {
    public:
        Lexer(atl::UTF8_Char_Iterator begin, atl::UTF8_Char_Iterator end): _begin(begin), _current(begin), _end(end) {}

        void ignore_whitespace_and_comments() {
            while(true) {
                char32 const next_char = peek();
                if(is_whitespace(next_char)) {
                    advance();
                    continue;
                }

                if(next_char == U'/') {
                    advance();
                    char32 const next_next_char = peek();
                    if(next_next_char == U'/') {
                        do {
                            advance();
                        } while(peek() != U'\n');
                        advance();
                        continue;
                    } else if(next_next_char == U'*') {
                        char32 c1;
                        char32 c2;
                        do {
                            c1 = peek();
                            advance();
                            c2 = peek();
                        } while(c1 != U'*' || c2 != U'/');
                        advance();
                        continue;
                    } else {
                        retard();
                        break;
                    }
                }

                break;
            }
        }

        bool try_match(char32 const c) {
            ignore_whitespace_and_comments();
            if(c == peek()) {
                advance();
                return true;
            } else {
                return false;
            }
        }

        Lexer_State get_current_state() const {
            return {_current};
        }

        void restore_state(Lexer_State state) {
            _current = state.current;
        }

        char32 peek() {
            if(_current != _end) {
                return *_current;
            } else {
                return eof_char32;
            }
        }

        void advance() {
            if(_current != _end) {
                ++_current;
            }
        }

        bool is_whitespace(char32 c) {
            // Files saved on Windows use \r\n as their line ending sequence.
            // We have to ignore it explicitly, otherwise the parser won't work on Linux
            // where we can't tell streams to translate \r\n to \n.
            return c == '\n' || c == '\r' || c == '\t' || c == ' ';
        }

        void retard() {
            if(_current != _begin) {
                --_current;
            }
        }

    private:
        atl::UTF8_Char_Iterator _begin;
        atl::UTF8_Char_Iterator _current;
        atl::UTF8_Char_Iterator _end;
    };

    struct Parser {
    private:
        bool try_null(Lexer& lexer) {
            Lexer_State const state_backup = lexer.get_current_state();
            bool const matched_null = lexer.try_match(U'n') && lexer.try_match(U'u') && lexer.try_match(U'l') && lexer.try_match(U'l');
            if(matched_null) {
                return true;
            } else {
                lexer.restore_state(state_backup);
                return false;
            }
        }

        bool try_boolean(Lexer& lexer, bool& out) {
            Lexer_State const state_backup = lexer.get_current_state();
            bool const matched_true = lexer.try_match(U't') && lexer.try_match(U'r') && lexer.try_match(U'u') && lexer.try_match(U'e');
            if(matched_true) {
                out = true;
                return true;
            }

            bool const matched_false =
                lexer.try_match(U'f') && lexer.try_match(U'a') && lexer.try_match(U'l') && lexer.try_match(U's') && lexer.try_match(U'e');
            if(matched_false) {
                out = false;
                return true;
            }

            lexer.restore_state(state_backup);
            return false;
        }

        bool try_number(Lexer& lexer, f64& out) {
            auto is_digit = [](char32 c) { return c >= U'0' && c <= U'9'; };

            Lexer_State const state_backup = lexer.get_current_state();
            lexer.ignore_whitespace_and_comments();
            atl::String number;
            if(char32 const next_char = lexer.peek(); next_char == '-' || next_char == U'+') {
                number += next_char;
                lexer.advance();
            }

            i64 pre_point_digits = 0;
            for(char32 next_char = lexer.peek(); is_digit(next_char); ++pre_point_digits) {
                number += next_char;
                lexer.advance();
                next_char = lexer.peek();
            }

            if(pre_point_digits == 0) {
                number += U'0';
            }

            i64 post_point_digits = 0;
            if(lexer.peek() == '.') {
                number += '.';
                lexer.advance();
                for(char32 next_char = lexer.peek(); is_digit(next_char); ++post_point_digits) {
                    number += next_char;
                    lexer.advance();
                    next_char = lexer.peek();
                }

                if(post_point_digits == 0) {
                    number += U'0';
                }
            }

            if(pre_point_digits == 0 && post_point_digits == 0) {
                lexer.restore_state(state_backup);
                return false;
            }

            if(char32 const e = lexer.peek(); e == U'e' || e == U'E') {
                number += 'E';
                lexer.advance();
                if(char32 const sign = lexer.peek(); sign == '-' || sign == U'+') {
                    number += sign;
                    lexer.advance();
                }

                i64 e_digits = 0;
                for(char32 next_char = lexer.peek(); is_digit(next_char); ++e_digits) {
                    number += next_char;
                    lexer.advance();
                    next_char = lexer.peek();
                }

                if(e_digits == 0) {
                    lexer.restore_state(state_backup);
                    return false;
                }
            }

            out = strtod(number.data(), nullptr);
            return true;
        }

        bool try_string(Lexer& lexer, atl::String& out) {
            Lexer_State const state_backup = lexer.get_current_state();
            char32 quote_char = eof_char32;
            if(lexer.try_match(U'"')) {
                quote_char = U'"';
            } else if(lexer.try_match(U'\'')) {
                quote_char = U'\'';
            } else {
                lexer.restore_state(state_backup);
                return false;
            }

            out.clear();
            while(true) {
                char32 const next_char = lexer.peek();
                if(next_char == quote_char) {
                    lexer.advance();
                    return true;
                } else if(next_char == U'\\') {
                    // Escaped character
                    out += next_char;
                    lexer.advance();
                    out += lexer.peek();
                    lexer.advance();
                } else if(next_char == eof_char32) {
                    out.clear();
                    return false;
                } else {
                    out += next_char;
                    lexer.advance();
                }
            }
        }

        bool try_unquoted_string(Lexer& lexer, atl::String& out) {
            auto is_valid_string_char = [](char32 c) -> bool { return (c >= 'a' & c <= 'z') | (c >= 'A' & c <= 'Z') | c == '$' | c == '_'; };
            auto is_digit = [](char32 c) -> bool { return c >= '0' & c <= '9'; };

            Lexer_State const state_backup = lexer.get_current_state();
            lexer.ignore_whitespace_and_comments();
            out.clear();
            if(char32 const next_char = lexer.peek(); is_valid_string_char(next_char)) {
                out += next_char;
                lexer.advance();
            } else {
                lexer.restore_state(state_backup);
                return false;
            }

            for(char32 next_char = lexer.peek(); is_valid_string_char(next_char) | is_digit(next_char);) {
                out += next_char;
                lexer.advance();
                next_char = lexer.peek();
            }

            return out.size_bytes() != 0;
        }

        bool try_array_elements(Lexer& lexer, _Array& array) {
            do {
                _Element element;
                if(try_value(lexer, element)) {
                    array.elements.emplace_back(atl::move(element));
                }
            } while(lexer.try_match(U','));
            return true;
        }

        bool try_array(Lexer& lexer, _Array& array) {
            Lexer_State const state_backup = lexer.get_current_state();
            if(!lexer.try_match(U'[')) {
                lexer.restore_state(state_backup);
                return false;
            }

            try_array_elements(lexer, array);

            if(!lexer.try_match(U']')) {
                lexer.restore_state(state_backup);
                return false;
            }

            return true;
        }

        bool try_member(Lexer& lexer, _Object& object) {
            Lexer_State const state_backup = lexer.get_current_state();
            Object_Member kv;
            if(try_string(lexer, kv.key)) {
            } else if(try_unquoted_string(lexer, kv.key)) {
            } else {
                lexer.restore_state(state_backup);
                return false;
            }

            if(!lexer.try_match(U':')) {
                lexer.restore_state(state_backup);
                return false;
            }

            if(!try_value(lexer, kv.value)) {
                lexer.restore_state(state_backup);
                return false;
            } else {
                object.elements.emplace(atl::hash(kv.key), atl::move(kv));
                return true;
            }
        }

        bool try_members(Lexer& lexer, _Object& object) {
            do {
                try_member(lexer, object);
            } while(lexer.try_match(U','));
            return true;
        }

        bool try_object(Lexer& lexer, _Object& object) {
            Lexer_State const state_backup = lexer.get_current_state();
            if(!lexer.try_match(U'{')) {
                lexer.restore_state(state_backup);
                return false;
            }

            try_members(lexer, object);

            if(!lexer.try_match(U'}')) {
                lexer.restore_state(state_backup);
                return false;
            }

            return true;
        }

        bool try_value(Lexer& lexer, _Element& element) {
            if(_Object object; try_object(lexer, object)) {
                element = atl::move(object);
                return true;
            } else if(_Array array; try_array(lexer, array)) {
                element = atl::move(array);
                return true;
            } else if(atl::String string; try_string(lexer, string)) {
                element = atl::move(string);
                return true;
            } else if(f64 number; try_number(lexer, number)) {
                element = number;
                return true;
            } else if(bool boolean; try_boolean(lexer, boolean)) {
                element = boolean;
                return true;
            } else if(try_null(lexer)) {
                element = null;
                return true;
            } else {
                return false;
            }
        }

    public:
        Document parse(atl::String_View sv) {
            Lexer lexer(sv.chars_begin(), sv.chars_end());
            Element root = nullptr;
            if(_Element element; try_value(lexer, element)) {
                root = new _Element(atl::move(element));
            }
            return Document(root);
        }
    };

    Document parse(atl::String_View string) {
        Parser parser;
        Document document = parser.parse(string);
        return document;
    }

    static void append_indent(atl::String& out, i64 indent_levels) {
        for(i64 i = 0; i < indent_levels; ++i) {
            out += u8"    ";
        }
    }

    static void stringify_element(Element const element, atl::String& out, bool const pretty_print, i64 const indent) {
        if(!element) {
            return;
        }

        switch(element_type(element)) {
            case Element_Type::object: {
                auto [object, _] = as_object(element);
                out += U'{';
                if(pretty_print) {
                    out += '\n';
                }

                for(Key_Value kv: object) {
                    if(pretty_print) {
                        append_indent(out, indent + 1);
                    }
                    out += U'"';
                    out += kv.key;
                    out += u8"\": ";
                    stringify_element(kv.value, out, pretty_print, indent + 2);
                    out += U',';
                    if(pretty_print) {
                        out += U'\n';
                    }
                }
                append_indent(out, indent);
                out += U'}';
            } break;

            case Element_Type::array: {
                auto [array, _] = as_array(element);
                out += U'[';
                bool first = true;
                for(Element element: array) {
                    if(!first) {
                        out += U',';
                        if(pretty_print) {
                            out += U' ';
                        }
                    }
                    first = false;
                    stringify_element(element, out, pretty_print, indent);
                }
                out += U']';
            } break;

            case Element_Type::string: {
                auto [string, _] = as_string(element);
                out += U'"';
                out += string;
                out += U'"';
            } break;

            case Element_Type::number_i64: {
                auto const [number, _] = as_i64(element);
                out += atl::to_string(number);
            } break;

            case Element_Type::number_f64: {
                auto const [number, _] = as_f64(element);
                out += atl::to_string(number);
            } break;

            case Element_Type::boolean: {
                auto [boolean, _] = as_boolean(element);
                if(boolean) {
                    out += u8"true";
                } else {
                    out += u8"false";
                }
            } break;

            case Element_Type::null: {
                out += u8"null";
            } break;
        }
    }

    atl::String stringify(Document const& document, bool const pretty_print) {
        Element root = document.get_root_element();
        atl::String out;
        stringify_element(root, out, pretty_print, 0);
        return out;
    }

    atl::Tuple<Object, bool> as_object(Element element) {
        return element->as_object();
    }

    atl::Tuple<Array, bool> as_array(Element element) {
        return element->as_array();
    }

    atl::Tuple<atl::String_View, bool> as_string(Element element) {
        return element->as_string();
    }

    atl::Tuple<i64, bool> as_i64(Element element) {
        return element->as_i64();
    }

    atl::Tuple<f64, bool> as_f64(Element element) {
        return element->as_f64();
    }

    atl::Tuple<bool, bool> as_boolean(Element element) {
        return element->as_boolean();
    }

    void assign_object(Element element) {
        *element = _Object();
    }

    void assign_array(Element element) {
        *element = _Array();
    }

    void assign_string(Element element, atl::String_View value) {
        *element = value;
    }

    void assign_string(Element element, atl::String&& value) {
        *element = atl::move(value);
    }

    void assign_i64(Element element, i64 value) {
        *element = value;
    }

    void assign_f64(Element element, f64 value) {
        *element = value;
    }

    void assign_boolean(Element element, bool value) {
        *element = value;
    }

    void assign_null(Element element) {
        *element = null;
    }

    bool exists(Object object, atl::String_View property) {
        u64 const hash = atl::hash(property);
        return object->elements.find(hash) != object->elements.end();
    }

    Element get_property(Object object, atl::String_View property) {
        u64 const hash = atl::hash(property);
        auto iter = object->elements.find(hash);
        if(iter != object->elements.end()) {
            _Element& element = iter->value.value;
            return &element;
        } else {
            return nullptr;
        }
    }

    Element create_property(Object object, atl::String_View property) {
        u64 const key = atl::hash(property);
        auto i = object->elements.emplace(key, atl::String(property), _Element(null));
        return &i->value.value;
    }

    void remove_property(Object object, atl::String_View property) {
        u64 const key = atl::hash(property);
        auto i = object->elements.find(key);
        if(i != object->elements.end()) {
            object->elements.erase(i);
        }
    }

    Element push_back(Array array) {
        _Element& element = array->elements.emplace_back();
        return &element;
    }

    void pop_back(Array array) {
        array->elements.pop_back();
    }

    Element insert(Array array, Array_Iterator iterator) {
        // TODO: Fix return value. Requires modification of vector's insert.
        array->elements.insert(iterator.element);
        return nullptr;
    }

    void erase(Array array, Array_Iterator iterator) {
        array->elements.erase(iterator.element, iterator.element + 1);
    }
} // namespace anton_engine::json
