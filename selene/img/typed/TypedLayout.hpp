// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_LAYOUT_HPP
#define SELENE_IMG_TYPED_LAYOUT_HPP

/// @file

#include <selene/img/common/PixelFormat.hpp>
#include <selene/img/pixel/PixelTraits.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

class TypedLayout
{
public:
  TypedLayout() noexcept
      : width{PixelLength{0}}
      , height{PixelLength{0}}
      , stride_bytes{Stride{0}}
  { }

  TypedLayout(PixelLength width_,
              PixelLength height_,
              Stride stride_bytes_ = Stride{0}) noexcept
      : width(width_)
      , height(height_)
      , stride_bytes(stride_bytes_)
  { }

  PixelLength width;
  PixelLength height;
  Stride stride_bytes;

  template <typename PixelType> std::ptrdiff_t nr_bytes_per_pixel() const noexcept { return PixelTraits<PixelType>::nr_bytes; }
  template <typename PixelType> std::ptrdiff_t row_bytes() const noexcept { return width * nr_bytes_per_pixel<PixelType>(); }
  std::ptrdiff_t total_bytes() const noexcept { return stride_bytes * height; }
  template <typename PixelType> bool is_packed() const noexcept { return stride_bytes == PixelTraits<PixelType>::nr_bytes * width; }
};

inline bool operator==(const TypedLayout& l, const TypedLayout& r)
{
  return l.width == r.width && l.height == r.height && l.stride_bytes == r.stride_bytes;
}

inline bool operator!=(const TypedLayout& l, const TypedLayout& r)
{
  return !(l == r);
}

}  // namespace sln

#endif  // SELENE_IMG_COMMON_TYPED_LAYOUT_HPP
