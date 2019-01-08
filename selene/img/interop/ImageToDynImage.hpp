// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_INTEROP_IMAGE_TO_DYN_IMAGE_HPP
#define SELENE_IMG_INTEROP_IMAGE_TO_DYN_IMAGE_HPP

/// @file

#include <selene/img/dynamic/DynImageView.hpp>
#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <stdexcept>

namespace sln {

template <typename PixelType>
DynImage to_dyn_image(Image<PixelType>&& img, PixelFormat new_pixel_format = PixelFormat::Invalid);

template <typename PixelType>
MutableDynImageView to_dyn_image_view(Image<PixelType>& img, PixelFormat new_pixel_format = PixelFormat::Invalid);

template <typename PixelType>
ConstantDynImageView to_dyn_image_view(const Image<PixelType>& img, PixelFormat new_pixel_format = PixelFormat::Invalid);

template <typename PixelType, ImageModifiability modifiability>
DynImageView<modifiability> to_dyn_image_view(const ImageView<PixelType, modifiability>& img_view, PixelFormat new_pixel_format = PixelFormat::Invalid);

// ----------

namespace impl {

template <typename PixelType, ImageModifiability modifiability>
PixelFormat check_img_to_dyn_img_compatibility(const ImageView<PixelType, modifiability>& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto pixel_format = PixelTraits<PixelType>::pixel_format;

  if (!img.is_valid())
  {
    throw std::runtime_error("Supplied image is not valid.");
  }

  // Override pixel format, if desired. Then perform compatibility check.
  new_pixel_format = (new_pixel_format == PixelFormat::Invalid) ? pixel_format : new_pixel_format;

  if (new_pixel_format != PixelFormat::Unknown && get_nr_channels(new_pixel_format) != nr_channels)
  {
    throw std::runtime_error("Mismatch in pixel format and number of channels.");
  }

  return new_pixel_format;
}

}  // namespace impl

/** \brief Converts a statically typed `Image<PixelType>` instance to a dynamically typed `DynImage` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, the pixel format, and the sample format of the resulting
 * `ImageData` instance are determined based on the `PixelTraits` of the `PixelType`.
 *
 * The `PixelFormat` may be overridden by specifying the `new_pixel_format` parameter as not `PixelFormat::Invalid`.
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * The `Image<PixelType>` instance `img` is moved from, i.e. it will be in a valid but unspecified state after the
 * function call.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `DynImage` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return A dynamically typed `DynImage` instance.
 */
template <typename PixelType>
DynImage to_dyn_image(Image<PixelType>&& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img.view(), new_pixel_format);

  const auto img_layout = img.layout();
  auto memory = img.relinquish_data_ownership();
  return DynImage{std::move(memory),
                  {img_layout.width, img_layout.height, nr_channels, nr_bytes_per_channel, img_layout.stride_bytes},
                  {new_pixel_format, sample_format}};
}

/** \brief Creates a dynamically typed `MutableDynImageView` view from a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, the pixel format, and the sample format of the resulting
 * `MutableDynImageView` instance are determined based on the `PixelTraits` of the `PixelType`.
 *
 * The `PixelFormat` may be overridden by specifying the `new_pixel_format` parameter as not `PixelFormat::Invalid`.
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * As the resulting `MutableDynImageView` is a non-owning view, the lifetime of the supplied `Image<PixelType>` instance
 * must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `MutableDynImageView` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return A dynamically typed `MutableDynImageView` instance.
 */
template <typename PixelType>
MutableDynImageView to_dyn_image_view(Image<PixelType>& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img.view(), new_pixel_format);

  return MutableDynImageView{img.byte_ptr(),
                             UntypedLayout{img.width(), img.height(), nr_channels, nr_bytes_per_channel, img.stride_bytes()},
                             UntypedImageSemantics{new_pixel_format, sample_format}};
}

/** \brief Creates a dynamically typed `ConstantDynImageView` view from a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `img` must be valid, i.e. `img.is_valid()` must return true. Otherwise this function
 * will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, the pixel format, and the sample format of the resulting
 * `ConstantDynImageView` instance are determined based on the `PixelTraits` of the `PixelType`.
 *
 * The `PixelFormat` may be overridden by specifying the `new_pixel_format` parameter as not `PixelFormat::Invalid`.
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * As the resulting `ConstantDynImageView` is a non-owning view, the lifetime of the supplied `Image<PixelType>`
 * instance must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance.
 * @param img The statically typed image.
 * @param pixel_format The pixel format of the `ConstantDynImageView` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return A dynamically typed `ConstantDynImageView` instance.
 */
template <typename PixelType>
ConstantDynImageView to_dyn_image_view(const Image<PixelType>& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img.view(), new_pixel_format);

  return ConstantDynImageView{img.byte_ptr(),
                              UntypedLayout{img.width(), img.height(), nr_channels, nr_bytes_per_channel, img.stride_bytes()},
                              UntypedImageSemantics{new_pixel_format, sample_format}};
}

/** \brief Creates a dynamically typed `DynImageView<modifiability>` view from a statically typed
 * `ImageView<PixelType, modifiability>` instance.
 *
 * Precondition: The supplied image `img_view` must be valid, i.e. `img_view.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, the pixel format, and the sample format of the resulting
 * `DynImageView<modifiability>` instance are determined based on the `PixelTraits` of the `PixelType`.
 *
 * The `PixelFormat` may be overridden by specifying the `new_pixel_format` parameter as not `PixelFormat::Invalid`.
 * If the desired `PixelFormat` is not `PixelFormat::Unknown`, a compatibility check will be performed between existing
 * and desired number of channels. If this check is negative, this function will throw a `std::runtime_error` exception.
 *
 * As the resulting `DynImageView<modifiability>` is a non-owning view, the lifetime of the supplied
 * `ImageView<PixelType, modifiability>` instance must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `ImageView<PixelType, modifiability>` instance.
 * @param img_view The statically typed image view.
 * @param pixel_format The pixel format of the `DynImageView<modifiability>` instance to be created. If unknown, can be
 *                     `PixelFormat::Unknown`.
 * @return A dynamically typed `DynImageView<modifiability>` instance.
 */
template <typename PixelType, ImageModifiability modifiability>
DynImageView<modifiability> to_dyn_image_view(const ImageView<PixelType, modifiability>& img_view, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img_view, new_pixel_format);

  return DynImageView<modifiability>{img_view.byte_ptr(),
                                     UntypedLayout{img_view.width(), img_view.height(), nr_channels, nr_bytes_per_channel, img_view.stride_bytes()},
                                     UntypedImageSemantics{new_pixel_format, sample_format}};
}

}  // namespace sln

#endif  // SELENE_IMG_INTEROP_IMAGE_TO_DYN_IMAGE_HPP
