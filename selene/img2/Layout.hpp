// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG2_LAYOUT_HPP
#define SELENE_IMG2_LAYOUT_HPP

/// @file

#include <selene/img2/PixelFormat.hpp>
#include <selene/img2/Types.hpp>

namespace sln2 {

class TypedLayout
{
public:
  TypedLayout(PixelLength width,
              PixelLength height,
              Stride stride_bytes = Stride{0}) noexcept
      : width_(width)
      , height_(height)
      , stride_bytes_(stride_bytes)
  { }

  PixelLength width() const noexcept { return width_; }
  PixelLength height() const noexcept { return height_; }
  Stride stride_bytes() const noexcept { return stride_bytes_; }

private:
  PixelLength width_;
  PixelLength height_;
  Stride stride_bytes_;
};

// -----------

class UntypedLayout
{
public:
  UntypedLayout(PixelLength width,
                PixelLength height,
                std::int16_t nr_channels,
                std::int16_t nr_bytes_per_channel,
                Stride stride_bytes = Stride{0}) noexcept
      : width_(width)
      , height_(height)
      , nr_channels_(nr_channels)
      , nr_bytes_per_channel_(nr_bytes_per_channel)
      , stride_bytes_(stride_bytes)
  { }

  PixelLength width() const noexcept { return width_; }
  PixelLength height() const noexcept { return height_; }
  std::int16_t nr_channels() const noexcept { return nr_channels_; }
  std::int16_t nr_bytes_per_channel() const noexcept { return nr_bytes_per_channel_; }
  Stride stride_bytes() const noexcept { return stride_bytes_; }

private:
  PixelLength width_;
  PixelLength height_;
  std::int16_t nr_channels_;
  std::int16_t nr_bytes_per_channel_;
  Stride stride_bytes_;
};

// -----------

class UntypedImageSemantics
{
public:
  explicit UntypedImageSemantics(PixelFormat pixel_format = PixelFormat::Unknown,
                                 SampleFormat sample_format = SampleFormat::Unknown)
      : pixel_format_(pixel_format)
      , sample_format_(sample_format)
  { }

  PixelFormat pixel_format() { return pixel_format_; }
  SampleFormat sample_format() { return sample_format_; }

private:
  PixelFormat pixel_format_;
  SampleFormat sample_format_;
};

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

  explicit DataPtr(const std::uint8_t* data) noexcept
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

  explicit DataPtr(std::uint8_t* data) noexcept
      : data_(data)
  { }

  std::uint8_t* data() noexcept { return data_; }

private:
  std::uint8_t* data_;
};

}  // namespace sln

#endif  // SELENE_IMG2_LAYOUT_HPP
