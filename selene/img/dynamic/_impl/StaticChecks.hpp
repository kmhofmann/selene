// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP
#define SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP

/// @file

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <type_traits>

namespace sln {

namespace impl {

template <typename> struct is_dyn_image : std::false_type {};
template <typename Allocator> struct is_dyn_image<DynImage<Allocator>> : std::true_type {};

template <typename DynImageOrView>
constexpr bool static_check_is_dyn_image()
{
  return is_dyn_image<DynImageOrView>::value;
}

template <typename DynImageOrView>
constexpr bool static_check_is_dyn_image_view()
{
  return std::is_same_v<DynImageOrView, DynImageView<ImageModifiability::Mutable>>
      || std::is_same_v<DynImageOrView, DynImageView<ImageModifiability::Constant>>;
}

template <typename DynImageOrView>
constexpr void static_check_is_dyn_image_or_view()
{
  static_assert(static_check_is_dyn_image<DynImageOrView>() || static_check_is_dyn_image_view<DynImageOrView>(),
                "Supplied type needs be either a sln::DynImage or a sln::DynImageView<>.");
}

template <typename DynImageOrView>
constexpr void static_check_is_dyn_image_or_mutable_view()
{
  static_assert(static_check_is_dyn_image<DynImageOrView>()
                || std::is_same_v<DynImageOrView, DynImageView<ImageModifiability::Mutable>>,
                "Supplied type needs be either a sln::DynImage or a sln::MutableDynImageView<>.");
}

} // namespace impl

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP
