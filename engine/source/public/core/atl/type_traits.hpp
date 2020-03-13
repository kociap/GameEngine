#ifndef CORE_ATL_TYPE_TRAITS_HPP_INCLUDE
#define CORE_ATL_TYPE_TRAITS_HPP_INCLUDE

// Since volatile has been deprecated in C++20, this implementation provides no traits for removing, adding or identifying volatile.

// atl::is_swappable, atl::is_noexcept_swappable are defined in <atl/utility>
// atl::is_iterator_wrapper is defined in <atl/iterators>

#include <core/atl/detail/traits_base.hpp>
#include <core/atl/detail/traits_function.hpp>
#include <core/atl/detail/traits_properties.hpp>
#include <core/atl/detail/traits_transformations.hpp>
#include <core/atl/detail/traits_utility.hpp>

#endif // !CORE_ATL_TYPE_TRAITS_HPP_INCLUDE
