#pragma once

#include <anton/array.hpp>
#include <core/exception.hpp>
#include <core/handle.hpp>
#include <core/integer_sequence_generator.hpp>

namespace anton_engine {
  template<typename T>
  class Resource_Manager {
  public:
    using iterator = typename anton::Array<T>::iterator;

    iterator begin();
    iterator end();

    Handle<T> add(T&&);
    T& get(Handle<T>);
    T const& get(Handle<T>) const;
    void remove(Handle<T>);

  private:
    anton::Array<T> resources;
    anton::Array<u64> identifiers;
  };
} // namespace anton_engine

namespace anton_engine {
  template<typename T>
  typename Resource_Manager<T>::iterator Resource_Manager<T>::begin()
  {
    return resources.begin();
  }

  template<typename T>
  typename Resource_Manager<T>::iterator Resource_Manager<T>::end()
  {
    return resources.end();
  }

  template<typename T>
  Handle<T> Resource_Manager<T>::add(T&& resource)
  {
    static Integer_Sequence_Generator id_generator;
    resources.emplace_back(ANTON_FWD(resource));
    u64 resource_id = id_generator.next();
    identifiers.push_back(resource_id);
    return {resource_id};
  }

  template<typename T>
  T& Resource_Manager<T>::get(Handle<T> handle)
  {
    for(typename anton::Array<T>::size_type i = 0; i < identifiers.size();
        ++i) {
      if(identifiers[i] == handle.value) {
        return resources[i];
      }
    }

    throw Exception(u8"Could not find resource with given handle");
  }

  template<typename T>
  T const& Resource_Manager<T>::get(Handle<T> handle) const
  {
    for(typename anton::Array<T>::size_type i = 0; i < identifiers.size();
        ++i) {
      if(identifiers[i] == handle.value) {
        return resources[i];
      }
    }

    throw Exception(u8"Could not find resource with given handle");
  }

  template<typename T>
  void Resource_Manager<T>::remove(Handle<T> handle)
  {
    for(typename anton::Array<T>::size_type i = 0; i < identifiers.size();
        ++i) {
      if(identifiers[i] == handle.value) {
        identifiers.erase_unsorted_unchecked(i);
        resources.erase_unsorted_unchecked(i);
        return;
      }
    }
  }
} // namespace anton_engine
