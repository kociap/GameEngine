#pragma once

#include <anton/aligned_buffer.hpp>
#include <anton/optional.hpp>
#include <anton/string.hpp>
#include <core/types.hpp>

namespace anton_engine::json {
  enum struct Element_Type {
    object,
    array,
    string,
    number_i64,
    number_f64,
    boolean,
    null,
  };

  struct _Element;
  using Element = _Element*;

  Element_Type element_type(Element element);

  struct _Object;
  using Object = _Object*;

  struct Key_Value {
    anton::String_View key;
    Element value;
  };

  struct Object_Iterator {
    anton::Aligned_Buffer<sizeof(void*) * 2, alignof(void*)> _;
  };

  Object_Iterator& operator++(Object_Iterator& iterator);
  [[nodiscard]] Key_Value operator*(Object_Iterator& iterator);
  [[nodiscard]] bool operator==(Object_Iterator const& lhs,
                                Object_Iterator const& rhs);

  [[nodiscard]] inline bool operator!=(Object_Iterator const& lhs,
                                       Object_Iterator const& rhs)
  {
    return !(lhs == rhs);
  }

  [[nodiscard]] Object_Iterator begin(Object object);
  [[nodiscard]] Object_Iterator end(Object object);

  struct _Array;
  using Array = _Array*;

  struct Array_Iterator {
    Array_Iterator& operator++();
    Array_Iterator& operator--();
    [[nodiscard]] Array_Iterator operator+(i64 offset);
    Array_Iterator& operator+=(i64 offset);
    [[nodiscard]] Array_Iterator operator-(i64 offset);
    Array_Iterator& operator-=(i64 offset);
    [[nodiscard]] Element operator*();
    [[nodiscard]] bool operator==(Array_Iterator const& rhs) const;
    [[nodiscard]] bool operator!=(Array_Iterator const& rhs) const;

    Element element;
  };

  [[nodiscard]] Array_Iterator begin(Array array);
  [[nodiscard]] Array_Iterator end(Array array);

  struct Document {
  public:
    // Constructs document with null as the root element.
    Document();
    Document(Element root);
    Document(Document&& document);
    Document& operator=(Document&& document);
    ~Document();
    [[nodiscard]] Element get_root_element() const;

  private:
    Element _root;
  };

  // parse
  // Parse json.
  //
  [[nodiscard]] Document parse(anton::String_View json);

  // stringify
  // Stringify json document.
  //
  [[nodiscard]] anton::String stringify(Document const& document,
                                        bool pretty_print);

  [[nodiscard]] anton::Optional<Object> as_object(Element element);
  [[nodiscard]] anton::Optional<Array> as_array(Element element);
  [[nodiscard]] anton::Optional<anton::String_View> as_string(Element element);
  [[nodiscard]] anton::Optional<i64> as_i64(Element element);
  [[nodiscard]] anton::Optional<f64> as_f64(Element element);
  [[nodiscard]] anton::Optional<bool> as_boolean(Element element);

  void assign_object(Element element);
  void assign_array(Element element);
  void assign_string(Element element, anton::String_View value);
  void assign_string(Element element, anton::String&& value);
  void assign_i64(Element element, i64 value);
  void assign_f64(Element element, f64 value);
  void assign_boolean(Element element, bool value);
  void assign_null(Element element);

  [[nodiscard]] bool exists(Object object, anton::String_View property);

  // get_property
  // Returns element or nullptr if property doesn't exist.
  //
  [[nodiscard]] Element get_property(Object object,
                                     anton::String_View property);

  // create_property
  // Creates a new property if it doesn't exist. Otherwise returns the existing one.
  //
  Element create_property(Object object, anton::String_View property);

  void remove_property(Object object, anton::String_View property);

  Element push_back(Array array);

  void pop_back(Array array);

  Element insert(Array array, Array_Iterator iterator);

  void erase(Array array, Array_Iterator iterator);
} // namespace anton_engine::json

namespace anton {
  template<>
  struct Tuple_Size<anton_engine::json::Key_Value>
    : Integral_Constant<usize, 2> {};

  template<>
  struct Tuple_Size<anton_engine::json::Key_Value const>
    : Integral_Constant<usize, 2> {};

  template<usize I>
  struct Tuple_Element<I, anton_engine::json::Key_Value> {
    static_assert(I == 0 || I == 1,
                  "anton_engine::json::Key_Value has only 2 elements");
    using type =
      conditional<I == 0, anton::String_View, anton_engine::json::Element>;
  };

  template<usize I>
  struct Tuple_Element<I, anton_engine::json::Key_Value const> {
    static_assert(I == 0 || I == 1,
                  "anton_engine::json::Key_Value has only 2 elements");
    using type = conditional<I == 0, anton::String_View const,
                             anton_engine::json::Element const>;
  };
} // namespace anton

// We provide std::tuple_size and std::tuple_element to enable structured bindings
namespace std {
  template<>
  struct tuple_size<anton_engine::json::Key_Value>
    : anton::Tuple_Size<anton_engine::json::Key_Value> {};

  template<>
  struct tuple_size<anton_engine::json::Key_Value const>
    : anton::Tuple_Size<anton_engine::json::Key_Value const> {};

  template<anton_engine::usize I>
  struct tuple_element<I, anton_engine::json::Key_Value>
    : anton::Tuple_Element<I, anton_engine::json::Key_Value> {};

  template<anton_engine::usize I>
  struct tuple_element<I, anton_engine::json::Key_Value const>
    : anton::Tuple_Element<I, anton_engine::json::Key_Value const> {};
} // namespace std
