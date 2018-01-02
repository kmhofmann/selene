// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP
#define SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP

/// @file

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>

#include <stdexcept>

namespace sln {

template <typename PixelType>
ImageData<> to_image_data(Image<PixelType>&& img, PixelFormat pixel_format);

template <typename PixelType>
ImageData<> to_image_data_view(Image<PixelType>& img, PixelFormat pixel_format);


// ----------
// Implementation:

/** \brief Converts a statically typed `Image<PixelType>` instance to a dynamically typed `ImageData` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the resulting `ImageData` instance
 * are determined based on the `PixelTraits` of the `PixelType`.
 *
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * The `Image<PixelType>` instance `img` is moved from, i.e. it will be in a valid but unspecified state after the
 * function call.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `ImageData` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return An `ImageData` instance.
 */
template <typename PixelType>
ImageData<> to_image_data(Image<PixelType>&& img, PixelFormat pixel_format)
{
  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (pixel_format != PixelFormat::Unknown && get_nr_channels(pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();

  const auto is_view = img.is_view();

  if (is_view)
  {
    const auto data = img.byte_ptr();
    return ImageData<>(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
                       sample_format);
  }
  else
  {
    auto data = img.relinquish_data_ownership();
    return ImageData<>(std::move(data), width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
                       sample_format);
  }
}

/** \brief Creates a dynamically typed `ImageData` view from a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the resulting `ImageData` instance
 * are determined based on the `PixelTraits` of the `PixelType`.
 *
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * As the resulting `ImageData` is a non-owning view, the lifetime of the supplied `Image<PixelType>` instance must be
 * equal to or exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `ImageData` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return An `ImageData` instance, which will be a view (i.e. `is_view() == true`).
 */
template <typename PixelType>
ImageData<> to_image_data_view(Image<PixelType>& img, PixelFormat pixel_format)
{
  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (pixel_format != PixelFormat::Unknown && get_nr_channels(pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();
  const auto data = img.byte_ptr();
  return ImageData<>(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format, sample_format);
}

/** \brief Creates a dynamically typed `ImageData` view from a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the resulting `ImageData` instance
 * are determined based on the `PixelTraits` of the `PixelType`.
 *
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * As the resulting `ImageData` is a non-owning view, the lifetime of the supplied `Image<PixelType>` instance must be
 * equal to or exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `ImageData` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return An `ImageData` instance, which will be a view (i.e. `is_view() == true`).
 */
template <typename PixelType>
ImageData<ImageDataStorage::Constant> to_image_data_view(const Image<PixelType>& img, PixelFormat pixel_format)
{
  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (pixel_format != PixelFormat::Unknown && get_nr_channels(pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();
  const auto data = img.byte_ptr();
  return ImageData<ImageDataStorage::Constant>(data, width, height, nr_channels, nr_bytes_per_channel, stride_bytes,
                                               pixel_format, sample_format);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_TO_IMAGE_DATA_HPP
