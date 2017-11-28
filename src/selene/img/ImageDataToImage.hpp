// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP
#define SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP

/// @file

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/Pixel.hpp>
#include <selene/img/PixelTraits.hpp>

#include <stdexcept>

namespace selene {
namespace img {

template <typename PixelType>
Image<PixelType> to_image(ImageData&& img_data);

template <typename PixelType>
Image<PixelType> to_image_view(ImageData& img_data);


// ----------
// Implementation:

/** \brief Converts a dynamically typed `ImageData` instance to a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img_data` must be valid, i.e. `img_data.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample type of the `ImageData` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * The `ImageData` instance `img_data` is moved from, i.e. it will be in a valid but unspecified state after the
 * function call.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param img_data The dynamically typed image.
 * @return An `Image<PixelType>` instance.
 */
template <typename PixelType>
Image<PixelType> to_image(ImageData&& img_data)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_type = PixelTraits<PixelType>::sample_type;

  if (!img_data.is_valid())
  {
    throw std::runtime_error("Supplied image data is not valid.");
  }

  if (img_data.nr_channels() != nr_channels || img_data.nr_bytes_per_channel() != nr_bytes_per_channel)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible nr of channels.");
  }

  if (img_data.sample_type() != SampleType::Unknown && img_data.sample_type() != sample_type)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible sample types.");
  }

  const auto width = img_data.width();
  const auto height = img_data.height();
  const auto stride_bytes = img_data.stride_bytes();

  const auto is_view = img_data.is_view();

  if (is_view)
  {
    const auto data = img_data.byte_ptr();
    return Image<PixelType>(data, width, height, stride_bytes);
  }
  else
  {
    auto data = img_data.relinquish_data_ownership();
    return Image<PixelType>(std::move(data), width, height, stride_bytes);
  }
}

/** \brief Creates a statically typed `Image<PixelType>` view from a dynamically typed `ImageData` instance.
 *
 * Precondition: The supplied image `img_data` must be valid, i.e. `img_data.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample type of the `ImageData` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param img_data The dynamically typed image.
 * @return An `Image<PixelType>` instance, which will be a view (i.e. `is_view() == true`).
 */
template <typename PixelType>
Image<PixelType> to_image_view(ImageData& img_data)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_type = PixelTraits<PixelType>::sample_type;

  if (!img_data.is_valid())
  {
    throw std::runtime_error("Supplied image data is not valid.");
  }

  if (img_data.nr_channels() != nr_channels || img_data.nr_bytes_per_channel() != nr_bytes_per_channel)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible nr of channels.");
  }

  if (img_data.sample_type() != SampleType::Unknown && img_data.sample_type() != sample_type)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible sample types.");
  }

  const auto width = img_data.width();
  const auto height = img_data.height();
  const auto stride_bytes = img_data.stride_bytes();
  const auto data = img_data.byte_ptr();
  return Image<PixelType>(data, width, height, stride_bytes);
}

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP