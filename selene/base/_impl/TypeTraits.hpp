// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_BASE_IMPL_TYPE_TRAITS_HPP
#define SELENE_BASE_IMPL_TYPE_TRAITS_HPP

#include <selene/base/_impl/ExplicitType.hpp>

#include <type_traits>

namespace sln {
namespace impl {

template <typename T>
struct is_integral : std::integral_constant<bool, std::is_integral_v<T>>
{ };

template <typename Value, typename Tag>
struct is_integral<ExplicitType<Value, Tag>> : std::integral_constant<bool, std::is_integral_v<Value>>
{ };

template <typename T> constexpr bool is_integral_v = is_integral<T>::value;

// ---

template <typename T>
struct is_floating_point : std::integral_constant<bool, std::is_floating_point_v<T>>
{ };

template <typename Value, typename Tag>
struct is_floating_point<ExplicitType<Value, Tag>>:  std::integral_constant<bool, std::is_floating_point_v<Value>>
{ };

template <typename T> constexpr bool is_floating_point_v = is_floating_point<T>::value;

}  // namespace impl
}  // namespace sln

#endif  // SELENE_BASE_IMPL_TYPE_TRAITS_HPP
