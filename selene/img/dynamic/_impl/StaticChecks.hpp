// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP
#define SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP

/// @file

#include <selene/img/dynamic/_impl/DynImageFwd.hpp>

#include <type_traits>

namespace sln::impl {

template <typename> struct is_dyn_image : std::false_type {};
template <typename Allocator> struct is_dyn_image<DynImage<Allocator>> : std::true_type {};
template <typename Img> constexpr bool is_dyn_image_v = is_dyn_image<Img>::value;

template <typename> struct is_dyn_image_view : std::false_type {};
template <ImageModifiability modifiability> struct is_dyn_image_view<DynImageView<modifiability>> : std::true_type {};
template <typename Img> constexpr bool is_dyn_image_view_v = is_dyn_image_view<Img>::value;

template <typename> struct is_constant_dyn_image_view : std::false_type {};
template <> struct is_constant_dyn_image_view<DynImageView<ImageModifiability::Constant>> : std::true_type {};
template <typename Img> constexpr bool is_constant_dyn_image_view_v = is_constant_dyn_image_view<Img>::value;

template <typename> struct is_mutable_dyn_image_view : std::false_type {};
template <> struct is_mutable_dyn_image_view<DynImageView<ImageModifiability::Mutable>> : std::true_type {};
template <typename Img> constexpr bool is_mutable_dyn_image_view_v = is_mutable_dyn_image_view<Img>::value;

// -----

template <typename DynImgOrView>
constexpr void static_assert_is_dyn_image_or_view()
{
  static_assert(is_dyn_image_v<DynImgOrView> || is_dyn_image_view_v<DynImgOrView>,
                "Supplied type needs be either a sln::DynImage or a sln::DynImageView<>.");
}

template <typename DynImgOrView>
constexpr void static_assert_is_dyn_image_or_mutable_view()
{
  static_assert(is_dyn_image_v<DynImgOrView> || is_mutable_dyn_image_view_v<DynImgOrView>,
                "Supplied type needs be either a sln::DynImage or a sln::MutableDynImageView<>.");
}

}  // namespace sln::impl

#endif  // SELENE_IMG_DYNAMIC_IMPL_STATIC_CHECKS_HPP
