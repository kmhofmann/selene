// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_VIEW_HPP
#define SELENE_IMG_OPS_VIEW_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

namespace sln {

template <typename DerivedSrc>
auto view(const ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc>
auto view(ImageBase<DerivedSrc>& img)
{
  return img.view();
}

template <typename DerivedSrc>
auto view(const ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelType, ImageModifiability::Constant>(byte_ptr, layout);
}

template <typename DerivedSrc>
auto view(ImageBase<DerivedSrc>& img, const BoundingBox& region)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;
  constexpr auto modifiability = ImageBase<DerivedSrc>::modifiability();

  const auto data_offset = Bytes(img.stride_bytes() * region.y0() + PixelTraits<PixelType>::nr_bytes * region.x0());
  const auto byte_ptr = img.byte_ptr() + data_offset;
  const auto layout = TypedLayout{region.width(), region.height(), img.stride_bytes()};

  return ImageView<PixelType, modifiability>(byte_ptr, layout);
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_VIEW_HPP
