// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_VIEW_HPP
#define SELENE_IMG_OPS_VIEW_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/pixel/Pixel.hpp>

#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <type_traits>

namespace sln {

// Functions after which the PixelType of the view will be unchanged:

template <typename DerivedSrc,
          typename = std::enable_if_t<is_image_type_v<DerivedSrc>>>
auto view(const ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc,
          typename = std::enable_if_t<is_image_type_v<DerivedSrc>>>
auto view(ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc,
          typename = std::enable_if_t<is_image_type_v<DerivedSrc>>>
auto view(const ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelTypeSrc>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelTypeSrc, ImageModifiability::Constant>(byte_ptr, layout);
}

template <typename DerivedSrc,
          typename = std::enable_if_t<is_image_type_v<DerivedSrc>>>
auto view(ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  constexpr auto modifiability = ImageBase<DerivedSrc>::modifiability();

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelTypeSrc>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelTypeSrc, modifiability>(byte_ptr, layout);
}


// Functions after which the PixelType of the view might be changed:

namespace impl {

template <typename PixelTypeSrc, typename PixelTypeDst>
void static_check_view_pixel_types()
{
  // Underlying element type and nr of channels both have to match; the pixel format has to match at least in the
  // nr of channels, or be PixelFormat::Unknown in either source or target.
  static_assert(std::is_same<typename PixelTraits<PixelTypeSrc>::Element,
                             typename PixelTraits<PixelTypeDst>::Element>::value,
                "Incompatible source and target pixel types");
  static_assert(PixelTraits<PixelTypeSrc>::nr_channels == PixelTraits<PixelTypeDst>::nr_channels,
                "Incompatible source and target pixel types");
  static_assert(get_nr_channels(PixelTraits<PixelTypeSrc>::pixel_format)
                == get_nr_channels(PixelTraits<PixelTypeDst>::pixel_format)
                || PixelTraits<PixelTypeSrc>::pixel_format == PixelFormat::Unknown
                || PixelTraits<PixelTypeDst>::pixel_format == PixelFormat::Unknown,
                "Incompatible source and target pixel types");

  // The sizes should then also be the same
  static_assert(sizeof(PixelTypeSrc) == sizeof(PixelTypeDst), "Incompatible source and target pixel types");
}

}  // namespace impl

// Ideally, these functions should also just be called view(), with SFINAE doing its magic to enable/disable
// these or the above overloads. No problem with Clang 7.0, but GCC 8.2.0 does not like this.
// TODO: Investigate later.

template <typename PixelTypeDst, typename DerivedSrc,
          typename = std::enable_if_t<is_pixel_type_v<PixelTypeDst>>>
auto view_with_pixel_type(const ImageBase<DerivedSrc>& img)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;

  impl::static_check_view_pixel_types<PixelTypeSrc, PixelTypeDst>();

  return ImageView<PixelTypeDst, ImageModifiability::Constant>(img.byte_ptr(), img.layout());
}

template <typename PixelTypeDst, typename DerivedSrc,
          typename = std::enable_if_t<is_pixel_type_v<PixelTypeDst>>>
auto view_with_pixel_type(ImageBase<DerivedSrc>& img)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;

  impl::static_check_view_pixel_types<PixelTypeSrc, PixelTypeDst>();

  constexpr auto modifiability = ImageBase<DerivedSrc>::modifiability();
  return ImageView<PixelTypeDst, modifiability>(img.byte_ptr(), img.layout());
}

template <typename PixelTypeDst, typename DerivedSrc,
          typename = std::enable_if_t<is_pixel_type_v<PixelTypeDst>>>
auto view_with_pixel_type(const ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;

  impl::static_check_view_pixel_types<PixelTypeSrc, PixelTypeDst>();

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelTypeSrc>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelTypeDst, ImageModifiability::Constant>(byte_ptr, layout);
}

template <typename PixelTypeDst, typename DerivedSrc,
          typename = std::enable_if_t<is_pixel_type_v<PixelTypeDst>>>
auto view_with_pixel_type(ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  constexpr auto modifiability = ImageBase<DerivedSrc>::modifiability();

  impl::static_check_view_pixel_types<PixelTypeSrc, PixelTypeDst>();

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelTypeSrc>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelTypeDst, modifiability>(byte_ptr, layout);
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_VIEW_HPP
