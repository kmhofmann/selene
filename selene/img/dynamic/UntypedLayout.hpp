// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_UNTYPED_LAYOUT_HPP
#define SELENE_IMG_DYNAMIC_UNTYPED_LAYOUT_HPP

/// @file

#include <selene/img/common/PixelFormat.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

class UntypedLayout
{
public:
  UntypedLayout() noexcept
      : width{PixelLength{0}}
      , height{PixelLength{0}}
      , nr_channels{0}
      , nr_bytes_per_channel{0}
      , stride_bytes{Stride{0}}
  { }

  UntypedLayout(PixelLength width_,
                PixelLength height_,
                std::int16_t nr_channels_,
                std::int16_t nr_bytes_per_channel_) noexcept
      : width(width_)
      , height(height_)
      , nr_channels(nr_channels_)
      , nr_bytes_per_channel(nr_bytes_per_channel_)
      , stride_bytes(width_ * nr_channels_ * nr_bytes_per_channel_)
  { }

  UntypedLayout(PixelLength width_,
                PixelLength height_,
                std::int16_t nr_channels_,
                std::int16_t nr_bytes_per_channel_,
                Stride stride_bytes_) noexcept
      : width(width_)
      , height(height_)
      , nr_channels(nr_channels_)
      , nr_bytes_per_channel(nr_bytes_per_channel_)
      , stride_bytes(stride_bytes_)
  { }

  PixelLength width;
  PixelLength height;
  std::int16_t nr_channels;
  std::int16_t nr_bytes_per_channel;
  Stride stride_bytes;

  std::ptrdiff_t nr_bytes_per_pixel() const noexcept
  {
    return nr_channels * nr_bytes_per_channel;
  }

  std::ptrdiff_t row_bytes() const noexcept
  {
    return width * nr_bytes_per_pixel();
  }

  std::ptrdiff_t total_bytes() const noexcept
  {
    return stride_bytes * height;
  }

  bool is_packed() const noexcept
  {
    return stride_bytes == width * nr_channels * nr_bytes_per_channel;
  }
};

inline bool operator==(const UntypedLayout& l, const UntypedLayout& r)
{
  return l.width == r.width && l.height == r.height
         && l.nr_channels == r.nr_channels && l.nr_bytes_per_channel == r.nr_bytes_per_channel
         && l.stride_bytes == r.stride_bytes;
}

inline bool operator!=(const UntypedLayout& l, const UntypedLayout& r)
{
  return !(l == r);
}

// -----------

class UntypedImageSemantics
{
public:
  UntypedImageSemantics() noexcept
      : pixel_format(PixelFormat::Unknown)
      , sample_format(SampleFormat::Unknown)
  { }

  UntypedImageSemantics(PixelFormat pixel_format_,
                        SampleFormat sample_format_)
      : pixel_format(pixel_format_)
      , sample_format(sample_format_)
  { }

  PixelFormat pixel_format;
  SampleFormat sample_format;
};

inline bool operator==(const UntypedImageSemantics& l, const UntypedImageSemantics& r)
{
  return l.pixel_format == r.pixel_format && l.sample_format == r.sample_format;
}

inline bool operator!=(const UntypedImageSemantics& l, const UntypedImageSemantics& r)
{
  return !(l == r);
}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_UNTYPED_LAYOUT_HPP
