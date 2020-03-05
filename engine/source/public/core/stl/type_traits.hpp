#ifndef CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE
#define CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE

// Since volatile has been deprecated in C++20, this implementation provides no traits for removing, adding or identifying volatile.

// anton_stl::is_swappable, anton_stl::is_noexcept_swappable are defined in <anton_stl/utility>
// anton_stl::is_iterator_wrapper is defined in <anton_stl/iterators>

#include <core/stl/detail/traits_base.hpp>
#include <core/stl/detail/traits_function.hpp>
#include <core/stl/detail/traits_properties.hpp>
#include <core/stl/detail/traits_transformations.hpp>
#include <core/stl/detail/traits_utility.hpp>

#endif // !CORE_ANTON_STL_TYPE_TRAITS_HPP_INCLUDE
