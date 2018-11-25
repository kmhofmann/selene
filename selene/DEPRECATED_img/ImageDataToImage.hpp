// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP
#define SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP

/// @file

#include <selene/DEPRECATED_img/Image.hpp>
#include <selene/DEPRECATED_img/ImageData.hpp>
#include <selene/DEPRECATED_img/Pixel.hpp>
#include <selene/DEPRECATED_img/PixelTraits.hpp>

#include <stdexcept>

namespace sln {

template <typename PixelType>
Image<PixelType> to_image(ImageData<>&& img_data);

template <typename PixelType>
Image<PixelType> to_image_view(ImageData<>& img_data);


// ----------
// Implementation:

/** \brief Converts a dynamically typed `ImageData` instance to a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img_data` must be valid, i.e. `img_data.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the `ImageData` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * The `ImageData` instance `img_data` is moved from, i.e. it will be in a valid but unspecified state after the
 * function call.
 *
 * If the `ImageData` instance is a view, then the returned `Image<PixelType>` instance will also be a view; image data
 * will not be copied.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param img_data The dynamically typed image.
 * @return An `Image<PixelType>` instance.
 */
template <typename PixelType>
Image<PixelType> to_image(ImageData<>&& img_data)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto pixel_format = PixelTraits<PixelType>::pixel_format;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (!img_data.is_valid())
  {
    throw std::runtime_error("Supplied image data is not valid.");
  }

  if (img_data.nr_channels() != nr_channels || img_data.nr_bytes_per_channel() != nr_bytes_per_channel)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible nr of channels.");
  }

  if (img_data.pixel_format() != PixelFormat::Unknown
      && pixel_format != PixelFormat::Unknown  // TODO: should conversion to Unknown be allowed?
      && img_data.pixel_format() != pixel_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible pixel formats.");
  }

  if (img_data.sample_format() != SampleFormat::Unknown && img_data.sample_format() != sample_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible sample formats.");
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
 * The number of channels, the number of bytes per channel, and the sample format of the `ImageData` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * As the resulting `Image<PixelType>` is a non-owning view, the lifetime of the supplied `ImageData` instance must be
 * equal to or exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param img_data The dynamically typed image.
 * @return An `Image<PixelType>` instance, which will be a view (i.e. `is_view() == true`).
 */
template <typename PixelType>
Image<PixelType> to_image_view(ImageData<>& img_data)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto pixel_format = PixelTraits<PixelType>::pixel_format;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (!img_data.is_valid())
  {
    throw std::runtime_error("Supplied image data is not valid.");
  }

  if (img_data.nr_channels() != nr_channels || img_data.nr_bytes_per_channel() != nr_bytes_per_channel)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible nr of channels.");
  }

  if (img_data.pixel_format() != PixelFormat::Unknown
      && pixel_format != PixelFormat::Unknown  // TODO: should conversion to Unknown be allowed?
      && img_data.pixel_format() != pixel_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: imcompatible pixel formats.");
  }

  if (img_data.sample_format() != SampleFormat::Unknown && img_data.sample_format() != sample_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible sample formats.");
  }

  const auto width = img_data.width();
  const auto height = img_data.height();
  const auto stride_bytes = img_data.stride_bytes();
  const auto data = img_data.byte_ptr();
  return Image<PixelType>(data, width, height, stride_bytes);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_DATA_TO_IMAGE_HPP
