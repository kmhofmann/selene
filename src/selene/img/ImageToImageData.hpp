// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP
#define SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>

#include <stdexcept>

namespace selene {
namespace img {

template <typename PixelType>
ImageData to_image_data(Image<PixelType>&& img, PixelFormat pixel_format)
{
  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_type = PixelTraits<PixelType>::sample_type;

  if (pixel_format != PixelFormat::Unknown && get_nr_channels(pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();
  const auto data = img.relinquish_data_ownership();
  return ImageData(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type,
                   true);
}

template <typename PixelType>
ImageData to_image_data_view(Image<PixelType>& img, PixelFormat pixel_format)
{
  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_type = PixelTraits<PixelType>::sample_type;

  if (pixel_format != PixelFormat::Unknown && get_nr_channels(pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();
  const auto data = img.byte_ptr();
  return ImageData(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_type,
                   false);
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP