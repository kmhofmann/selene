// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMPL_STATIC_CHECKS_HPP
#define SELENE_IMG_TYPED_IMPL_STATIC_CHECKS_HPP

/// @file

#include <selene/img/typed/_impl/ImageFwd.hpp>

#include <type_traits>

namespace sln::impl {

template <typename> struct is_image : std::false_type {};
template <typename PixelType, typename Allocator> struct is_image<Image<PixelType, Allocator>> : std::true_type {};
template <typename Img> constexpr bool is_image_v = is_image<Img>::value;

template <typename> struct is_image_view : std::false_type {};
template <typename PixelType, ImageModifiability modifiability> struct is_image_view<ImageView<PixelType, modifiability>> : std::true_type {};
template <typename Img> constexpr bool is_image_view_v = is_image_view<Img>::value;

template <typename> struct is_constant_image_view : std::false_type {};
template <typename PixelType> struct is_constant_image_view<ImageView<PixelType, ImageModifiability::Constant>> : std::true_type {};
template <typename Img> constexpr bool is_constant_image_view_v = is_constant_image_view<Img>::value;

template <typename> struct is_mutable_image_view : std::false_type {};
template <typename PixelType> struct is_mutable_image_view<ImageView<PixelType, ImageModifiability::Mutable>> : std::true_type {};
template <typename Img> constexpr bool is_mutable_image_view_v = is_mutable_image_view<Img>::value;

template <typename ImageType> struct is_image_type : std::false_type {};
template <typename PixelType, typename Allocator> struct is_image_type<Image<PixelType, Allocator>> : std::true_type {};
template <typename PixelType, ImageModifiability modifiability> struct is_image_type<ImageView<PixelType, modifiability>> : std::true_type {};
template <typename ImageType> constexpr bool is_image_type_v = is_image_type<ImageType>::value;

// -----

template <typename ImgOrView>
constexpr void static_assert_is_image_or_view()
{
  static_assert(is_image_v<ImgOrView> || is_image_view_v<ImgOrView>,
                "Supplied type needs be either a sln::Image<> or a sln::ImageView<>.");
}

template <typename ImgOrView>
constexpr void static_assert_is_image_or_mutable_view()
{
  static_assert(is_image_v<ImgOrView> || is_mutable_image_view_v<ImgOrView>,
                "Supplied type needs be either a sln::Image<> or a sln::ImageView<>.");
}

}  // namespace sln::impl

#endif  // SELENE_IMG_TYPED_IMPL_STATIC_CHECKS_HPP
