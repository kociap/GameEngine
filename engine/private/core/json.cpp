#include <core/json.hpp>

#include <anton/array.hpp>
#include <anton/assert.hpp>
#include <anton/filesystem.hpp>
#include <anton/flat_hash_map.hpp>
#include <anton/string.hpp>
#include <core/types.hpp>

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
    _Element(anton::String_View string);
    _Element(anton::String&& string);
    _Element(i64 number);
    _Element(f64 number);
    _Element(bool boolean);
    _Element(Null null);
    _Element(_Element&& element);
    _Element& operator=(_Object&& object);
    _Element& operator=(_Array&& array);
    _Element& operator=(anton::String_View string);
    _Element& operator=(anton::String&& string);
    _Element& operator=(i64 number);
    _Element& operator=(f64 number);
    _Element& operator=(bool boolean);
    _Element& operator=(Null null);
    _Element& operator=(_Element&& element);
    ~_Element();

    [[nodiscard]] Element_Type element_type() const;

    [[nodiscard]] anton::Optional<Object> as_object() const;
    [[nodiscard]] anton::Optional<Array> as_array() const;
    [[nodiscard]] anton::Optional<anton::String_View> as_string() const;
    [[nodiscard]] anton::Optional<i64> as_i64() const;
    [[nodiscard]] anton::Optional<f64> as_f64() const;
    [[nodiscard]] anton::Optional<bool> as_boolean() const;

  private:
    Element_Type _type;
    void* _value;

    void destroy_value();
  };

  struct Object_Member {
    anton::String key;
    _Element value;

    Object_Member() = default;
    Object_Member(anton::String const& s, _Element&& e)
      : key(s), value(ANTON_MOV(e))
    {
    }
    Object_Member(anton::String&& s, _Element&& e)
      : key(ANTON_MOV(s)), value(ANTON_MOV(e))
    {
    }
  };

  struct _Object {
    anton::Flat_Hash_Map<u64, Object_Member> elements;
  };

  Object_Iterator& operator++(Object_Iterator& iterator)
  {
    auto& i = *(anton::Flat_Hash_Map<u64, Object_Member>::iterator*)&iterator;
    ++i;
    return iterator;
  }

  Key_Value operator*(Object_Iterator& iterator)
  {
    auto& i = *(anton::Flat_Hash_Map<u64, Object_Member>::iterator*)&iterator;
    Object_Member& member = i->value;
    return {member.key, &member.value};
  }

  bool operator==(Object_Iterator const& lhs, Object_Iterator const& rhs)
  {
    auto& _lhs = *(anton::Flat_Hash_Map<u64, Object_Member>::iterator*)&lhs;
    auto& _rhs = *(anton::Flat_Hash_Map<u64, Object_Member>::iterator*)&rhs;
    return _lhs == _rhs;
  }

  Object_Iterator begin(Object object)
  {
    Object_Iterator iterator;
    ::new(&iterator) anton::Flat_Hash_Map<u64, Object_Member>::iterator(
      object->elements.begin());
    return iterator;
  }

  Object_Iterator end(Object object)
  {
    Object_Iterator iterator;
    ::new(&iterator) anton::Flat_Hash_Map<u64, Object_Member>::iterator(
      object->elements.end());
    return iterator;
  }

  struct _Array {
    anton::Array<_Element> elements;
  };

  Array_Iterator& Array_Iterator::operator++()
  {
    ++element;
    return *this;
  }

  Array_Iterator& Array_Iterator::operator--()
  {
    --element;
    return *this;
  }

  Array_Iterator Array_Iterator::operator+(i64 offset)
  {
    Array_Iterator iterator;
    iterator.element = element + offset;
    return iterator;
  }

  Array_Iterator& Array_Iterator::operator+=(i64 offset)
  {
    element += offset;
    return *this;
  }

  Array_Iterator Array_Iterator::operator-(i64 offset)
  {
    Array_Iterator iterator;
    iterator.element = element - offset;
    return iterator;
  }

  Array_Iterator& Array_Iterator::operator-=(i64 offset)
  {
    element -= offset;
    return *this;
  }

  Element Array_Iterator::operator*()
  {
    return element;
  }

  bool Array_Iterator::operator==(Array_Iterator const& rhs) const
  {
    return element == rhs.element;
  }

  bool Array_Iterator::operator!=(Array_Iterator const& rhs) const
  {
    return element != rhs.element;
  }

  Array_Iterator begin(Array array)
  {
    return {array->elements.begin()};
  }

  Array_Iterator end(Array array)
  {
    return {array->elements.end()};
  }

  _Element::_Element(): _type(Element_Type::null), _value(nullptr) {}

  _Element::_Element(_Object&& object)
  {
    _type = Element_Type::object;
    _value = new _Object(ANTON_MOV(object));
  }

  _Element::_Element(_Array&& array)
  {
    _type = Element_Type::array;
    _value = new _Array(ANTON_MOV(array));
  }

  _Element::_Element(anton::String_View string)
    : _type(Element_Type::string), _value(new anton::String(string))
  {
  }

  _Element::_Element(anton::String&& string)
    : _type(Element_Type::string), _value(new anton::String(ANTON_MOV(string)))
  {
  }

  _Element::_Element(i64 number)
    : _type(Element_Type::number_i64), _value((void*)(*(u64*)&number))
  {
  }

  _Element::_Element(f64 number)
    : _type(Element_Type::number_f64), _value((void*)(*(u64*)&number))
  {
  }

  _Element::_Element(bool boolean)
    : _type(Element_Type::boolean), _value((void*)boolean)
  {
  }

  _Element::_Element(Null): _type(Element_Type::null), _value(nullptr) {}

  _Element::_Element(_Element&& element)
    : _type(element._type), _value(element._value)
  {
    element._type = Element_Type::null;
    element._value = nullptr;
  }

  _Element& _Element::operator=(_Object&& object)
  {
    destroy_value();
    _type = Element_Type::object;
    _value = new _Object(ANTON_MOV(object));
    return *this;
  }

  _Element& _Element::operator=(_Array&& array)
  {
    destroy_value();
    _type = Element_Type::array;
    _value = new _Array(ANTON_MOV(array));
    return *this;
  }

  _Element& _Element::operator=(anton::String_View string)
  {
    destroy_value();
    _type = Element_Type::string;
    _value = new anton::String(string);
    return *this;
  }

  _Element& _Element::operator=(anton::String&& string)
  {
    destroy_value();
    _type = Element_Type::string;
    _value = new anton::String(ANTON_MOV(string));
    return *this;
  }

  _Element& _Element::operator=(i64 number)
  {
    destroy_value();
    _type = Element_Type::number_i64;
    _value = (void*)(*(u64*)&number);
    return *this;
  }

  _Element& _Element::operator=(f64 number)
  {
    destroy_value();
    _type = Element_Type::number_f64;
    _value = (void*)(*(u64*)&number);
    return *this;
  }

  _Element& _Element::operator=(bool boolean)
  {
    destroy_value();
    _type = Element_Type::boolean;
    _value = (void*)boolean;
    return *this;
  }

  _Element& _Element::operator=(Null)
  {
    destroy_value();
    _type = Element_Type::null;
    _value = nullptr;
    return *this;
  }

  _Element& _Element::operator=(_Element&& element)
  {
    anton::swap(_type, element._type);
    anton::swap(_value, element._value);
    return *this;
  }

  _Element::~_Element()
  {
    destroy_value();
  }

  Element_Type _Element::element_type() const
  {
    return _type;
  }

  anton::Optional<Object> _Element::as_object() const
  {
    if(_type != Element_Type::object) {
      return anton::null_optional;
    }
    Object object = (Object)_value;
    return object;
  }

  anton::Optional<Array> _Element::as_array() const
  {
    if(_type != Element_Type::array) {
      return anton::null_optional;
    }
    Array array = (Array)_value;
    return array;
  }

  anton::Optional<anton::String_View> _Element::as_string() const
  {
    if(_type != Element_Type::string) {
      return anton::null_optional;
    }
    anton::String const& string = *(anton::String*)_value;
    return string;
  }

  anton::Optional<i64> _Element::as_i64() const
  {
    if(_type != Element_Type::number_i64) {
      return anton::null_optional;
    }
    i64 const number = *(i64*)&_value;
    return number;
  }

  anton::Optional<f64> _Element::as_f64() const
  {
    if(_type != Element_Type::number_f64) {
      return anton::null_optional;
    }
    f64 const number = *(f64*)&_value;
    return number;
  }

  anton::Optional<bool> _Element::as_boolean() const
  {
    if(_type != Element_Type::boolean) {
      return anton::null_optional;
    }
    bool const boolean = (bool)_value;
    return boolean;
  }

  void _Element::destroy_value()
  {
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
      anton::String* string = (anton::String*)_value;
      delete string;
    } break;

    default:
      break;
    }
  }

  Element_Type element_type(Element element)
  {
    return element->element_type();
  }

  Document::Document(): _root(new _Element(null)) {}

  Document::Document(Element root): _root(root) {}

  Document::Document(Document&& document): _root(document._root)
  {
    document._root = nullptr;
  }

  Document& Document::operator=(Document&& document)
  {
    anton::swap(_root, document._root);
    return *this;
  }

  Document::~Document()
  {
    delete _root;
  }

  Element Document::get_root_element() const
  {
    return _root;
  }

  struct Lexer_State {
    anton::UTF8_Char_Iterator current;
  };

  struct Lexer {
  public:
    Lexer(anton::UTF8_Char_Iterator begin, anton::UTF8_Char_Iterator end)
      : _begin(begin), _current(begin), _end(end)
    {
    }

    void ignore_whitespace_and_comments()
    {
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

    bool try_match(char32 const c)
    {
      ignore_whitespace_and_comments();
      if(c == peek()) {
        advance();
        return true;
      } else {
        return false;
      }
    }

    Lexer_State get_current_state() const
    {
      return {_current};
    }

    void restore_state(Lexer_State state)
    {
      _current = state.current;
    }

    char32 peek()
    {
      if(_current != _end) {
        return *_current;
      } else {
        return anton::eof_char32;
      }
    }

    void advance()
    {
      if(_current != _end) {
        ++_current;
      }
    }

    bool is_whitespace(char32 c)
    {
      // Files saved on Windows use \r\n as their line ending sequence.
      // We have to ignore it explicitly, otherwise the parser won't work on Linux
      // where we can't tell streams to translate \r\n to \n.
      return c == '\n' || c == '\r' || c == '\t' || c == ' ';
    }

    void retard()
    {
      if(_current != _begin) {
        --_current;
      }
    }

  private:
    anton::UTF8_Char_Iterator _begin;
    anton::UTF8_Char_Iterator _current;
    anton::UTF8_Char_Iterator _end;
  };

  struct Parser {
  private:
    bool try_null(Lexer& lexer)
    {
      Lexer_State const state_backup = lexer.get_current_state();
      bool const matched_null = lexer.try_match(U'n') &&
                                lexer.try_match(U'u') &&
                                lexer.try_match(U'l') && lexer.try_match(U'l');
      if(matched_null) {
        return true;
      } else {
        lexer.restore_state(state_backup);
        return false;
      }
    }

    bool try_boolean(Lexer& lexer, bool& out)
    {
      Lexer_State const state_backup = lexer.get_current_state();
      bool const matched_true = lexer.try_match(U't') &&
                                lexer.try_match(U'r') &&
                                lexer.try_match(U'u') && lexer.try_match(U'e');
      if(matched_true) {
        out = true;
        return true;
      }

      bool const matched_false =
        lexer.try_match(U'f') && lexer.try_match(U'a') &&
        lexer.try_match(U'l') && lexer.try_match(U's') && lexer.try_match(U'e');
      if(matched_false) {
        out = false;
        return true;
      }

      lexer.restore_state(state_backup);
      return false;
    }

    bool try_number(Lexer& lexer, f64& out)
    {
      auto is_digit = [](char32 c) { return c >= U'0' && c <= U'9'; };

      Lexer_State const state_backup = lexer.get_current_state();
      lexer.ignore_whitespace_and_comments();
      anton::String number;
      if(char32 const next_char = lexer.peek();
         next_char == '-' || next_char == U'+') {
        number += next_char;
        lexer.advance();
      }

      i64 pre_point_digits = 0;
      for(char32 next_char = lexer.peek(); is_digit(next_char);
          ++pre_point_digits) {
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
        for(char32 next_char = lexer.peek(); is_digit(next_char);
            ++post_point_digits) {
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

    bool try_string(Lexer& lexer, anton::String& out)
    {
      Lexer_State const state_backup = lexer.get_current_state();
      char32 quote_char = anton::eof_char32;
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
        } else if(next_char == anton::eof_char32) {
          out.clear();
          return false;
        } else {
          out += next_char;
          lexer.advance();
        }
      }
    }

    bool try_unquoted_string(Lexer& lexer, anton::String& out)
    {
      auto is_valid_string_char = [](char32 c) -> bool {
        return (c >= 'a' & c <= 'z') | (c >= 'A' & c <= 'Z') | c == '$' |
               c == '_';
      };
      auto is_digit = [](char32 c) -> bool { return c >= '0' & c <= '9'; };

      Lexer_State const state_backup = lexer.get_current_state();
      lexer.ignore_whitespace_and_comments();
      out.clear();
      if(char32 const next_char = lexer.peek();
         is_valid_string_char(next_char)) {
        out += next_char;
        lexer.advance();
      } else {
        lexer.restore_state(state_backup);
        return false;
      }

      for(char32 next_char = lexer.peek();
          is_valid_string_char(next_char) | is_digit(next_char);) {
        out += next_char;
        lexer.advance();
        next_char = lexer.peek();
      }

      return out.size_bytes() != 0;
    }

    bool try_array_elements(Lexer& lexer, _Array& array)
    {
      do {
        _Element element;
        if(try_value(lexer, element)) {
          array.elements.emplace_back(ANTON_MOV(element));
        }
      } while(lexer.try_match(U','));
      return true;
    }

    bool try_array(Lexer& lexer, _Array& array)
    {
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

    bool try_member(Lexer& lexer, _Object& object)
    {
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
        object.elements.emplace(anton::hash(kv.key), ANTON_MOV(kv));
        return true;
      }
    }

    bool try_members(Lexer& lexer, _Object& object)
    {
      do {
        try_member(lexer, object);
      } while(lexer.try_match(U','));
      return true;
    }

    bool try_object(Lexer& lexer, _Object& object)
    {
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

    bool try_value(Lexer& lexer, _Element& element)
    {
      if(_Object object; try_object(lexer, object)) {
        element = ANTON_MOV(object);
        return true;
      } else if(_Array array; try_array(lexer, array)) {
        element = ANTON_MOV(array);
        return true;
      } else if(anton::String string; try_string(lexer, string)) {
        element = ANTON_MOV(string);
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
    Document parse(anton::String_View sv)
    {
      Lexer lexer(sv.chars_begin(), sv.chars_end());
      Element root = nullptr;
      if(_Element element; try_value(lexer, element)) {
        root = new _Element(ANTON_MOV(element));
      }
      return Document(root);
    }
  };

  Document parse(anton::String_View string)
  {
    Parser parser;
    Document document = parser.parse(string);
    return document;
  }

  static void append_indent(anton::String& out, i64 indent_levels)
  {
    for(i64 i = 0; i < indent_levels; ++i) {
      out += u8"    ";
    }
  }

  static void stringify_element(Element const element, anton::String& out,
                                bool const pretty_print, i64 const indent)
  {
    if(!element) {
      return;
    }

    switch(element_type(element)) {
    case Element_Type::object: {
      auto typed_element = as_object(element);
      Object const object = *typed_element;
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
      auto typed_element = as_array(element);
      Array const array = *typed_element;
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
      auto typed_element = as_string(element);
      auto string = *typed_element;
      out += U'"';
      out += string;
      out += U'"';
    } break;

    case Element_Type::number_i64: {
      auto typed_element = as_i64(element);
      auto const number = *typed_element;
      out += anton::to_string(number);
    } break;

    case Element_Type::number_f64: {
      auto typed_element = as_f64(element);
      auto const number = *typed_element;
      out += anton::to_string(number);
    } break;

    case Element_Type::boolean: {
      auto typed_element = as_boolean(element);
      auto boolean = *typed_element;
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

  anton::String stringify(Document const& document, bool const pretty_print)
  {
    Element root = document.get_root_element();
    anton::String out;
    stringify_element(root, out, pretty_print, 0);
    return out;
  }

  anton::Optional<Object> as_object(Element element)
  {
    return element->as_object();
  }

  anton::Optional<Array> as_array(Element element)
  {
    return element->as_array();
  }

  anton::Optional<anton::String_View> as_string(Element element)
  {
    return element->as_string();
  }

  anton::Optional<i64> as_i64(Element element)
  {
    return element->as_i64();
  }

  anton::Optional<f64> as_f64(Element element)
  {
    return element->as_f64();
  }

  anton::Optional<bool> as_boolean(Element element)
  {
    return element->as_boolean();
  }

  void assign_object(Element element)
  {
    *element = _Object();
  }

  void assign_array(Element element)
  {
    *element = _Array();
  }

  void assign_string(Element element, anton::String_View value)
  {
    *element = value;
  }

  void assign_string(Element element, anton::String&& value)
  {
    *element = ANTON_MOV(value);
  }

  void assign_i64(Element element, i64 value)
  {
    *element = value;
  }

  void assign_f64(Element element, f64 value)
  {
    *element = value;
  }

  void assign_boolean(Element element, bool value)
  {
    *element = value;
  }

  void assign_null(Element element)
  {
    *element = null;
  }

  bool exists(Object object, anton::String_View property)
  {
    u64 const hash = anton::hash(property);
    return object->elements.find(hash) != object->elements.end();
  }

  Element get_property(Object object, anton::String_View property)
  {
    u64 const hash = anton::hash(property);
    auto iter = object->elements.find(hash);
    if(iter != object->elements.end()) {
      _Element& element = iter->value.value;
      return &element;
    } else {
      return nullptr;
    }
  }

  Element create_property(Object object, anton::String_View property)
  {
    u64 const key = anton::hash(property);
    auto i =
      object->elements.emplace(key, anton::String(property), _Element(null));
    return &i->value.value;
  }

  void remove_property(Object object, anton::String_View property)
  {
    u64 const key = anton::hash(property);
    auto i = object->elements.find(key);
    if(i != object->elements.end()) {
      object->elements.erase(i);
    }
  }

  Element push_back(Array array)
  {
    _Element& element = array->elements.emplace_back();
    return &element;
  }

  void pop_back(Array array)
  {
    array->elements.pop_back();
  }

  Element insert(Array array, Array_Iterator iterator)
  {
    auto element = array->elements.insert(iterator.element, _Element{});
    return element;
  }

  void erase(Array array, Array_Iterator iterator)
  {
    array->elements.erase(iterator.element, iterator.element + 1);
  }
} // namespace anton_engine::json
