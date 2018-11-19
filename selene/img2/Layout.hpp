// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_LAYOUT_HPP
#define SELENE_IMG2_LAYOUT_HPP

/// @file

#include <selene/img2/PixelFormat.hpp>
#include <selene/img2/PixelTraits.hpp>
#include <selene/img2/Types.hpp>

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

// -----------

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
                std::int16_t nr_bytes_per_channel_,
                Stride stride_bytes_ = Stride{0}) noexcept
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

  std::ptrdiff_t nr_bytes_per_pixel() const noexcept { return nr_channels * nr_bytes_per_channel; }
  std::ptrdiff_t row_bytes() const noexcept { return width * nr_bytes_per_pixel(); }
  std::ptrdiff_t total_bytes() const noexcept { return stride_bytes * height; }
  bool is_packed() const noexcept { return stride_bytes == nr_channels * nr_bytes_per_channel * width; }
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

// -----------

enum class ImageModifiability
{
  Constant,
  Mutable
};

// -----------

template <ImageModifiability>
class DataPtr;

// ---

template <>
class DataPtr<ImageModifiability::Constant>
{
public:
  using Type = const std::uint8_t*;
  using ConstType = const std::uint8_t*;

  DataPtr() noexcept
      : data_(nullptr)
  { }

  /*explicit*/ DataPtr(const std::uint8_t* data) noexcept
      : data_(data)
  { }

  const std::uint8_t* data() const noexcept { return data_; }

private:
  const std::uint8_t* data_;
};

// ---

template <>
class DataPtr<ImageModifiability::Mutable>
{
public:
  using Type = std::uint8_t*;
  using ConstType = const std::uint8_t*;

  DataPtr() noexcept
      : data_(nullptr)
  { }

  /*explicit*/ DataPtr(std::uint8_t* data) noexcept
      : data_(data)
  { }

  std::uint8_t* data() noexcept { return data_; }
  std::uint8_t* data() const noexcept { return data_; }

private:
  std::uint8_t* data_;
};

}  // namespace sln

#endif  // SELENE_IMG2_LAYOUT_HPP
