// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_INTEROP_DYN_IMAGE_TO_IMAGE_HPP
#define SELENE_IMG_INTEROP_DYN_IMAGE_TO_IMAGE_HPP

/// @file

#include <selene/img/dynamic/DynImageView.hpp>
#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <stdexcept>

namespace sln {

/// \addtogroup group-img-interop
/// @{

template <typename PixelType>
Image<PixelType> to_image(DynImage&& dyn_img);

template <typename PixelType>
MutableImageView<PixelType> to_image_view(DynImage& dyn_img);

template <typename PixelType>
ConstantImageView<PixelType> to_image_view(const DynImage& dyn_img);

template <typename PixelType, ImageModifiability modifiability>
ImageView<PixelType, modifiability> to_image_view(const DynImageView<modifiability>& dyn_img_view);

/// @}

// ----------

namespace impl {

template <typename PixelType, ImageModifiability modifiability>
void check_dyn_img_to_img_compatibility(const DynImageView<modifiability>& dyn_img)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto pixel_format = PixelTraits<PixelType>::pixel_format;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  if (!dyn_img.is_valid())
  {
    throw std::runtime_error("Supplied image data is not valid.");
  }

  if (dyn_img.nr_channels() != nr_channels || dyn_img.nr_bytes_per_channel() != nr_bytes_per_channel)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible nr of channels.");
  }

  if (dyn_img.pixel_format() != PixelFormat::Unknown
      && pixel_format != PixelFormat::Unknown
      && dyn_img.pixel_format() != pixel_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible pixel formats.");
  }

  if (dyn_img.sample_format() != SampleFormat::Unknown && dyn_img.sample_format() != sample_format)
  {
    throw std::runtime_error("Cannot convert ImageData to desired Image<> format: incompatible sample formats.");
  }
}

}  // namespace impl

/** \brief Converts a dynamically typed `DynImage` instance to a statically typed `Image<PixelType>` instance.
 *
 * Precondition: The supplied image `dyn_img` must be valid, i.e. `dyn_img.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the `DynImage` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * The `DynImage` instance `dyn_img` is moved from, i.e. it will be in a valid but unspecified state after the
 * function call.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param dyn_img The dynamically typed image.
 * @return An `Image<PixelType>` instance.
 */
template <typename PixelType>
Image<PixelType> to_image(DynImage&& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img.view());

  const auto dyn_img_layout = dyn_img.layout();
  auto memory = dyn_img.relinquish_data_ownership();
  return Image<PixelType>{std::move(memory),
                          TypedLayout{dyn_img_layout.width, dyn_img_layout.height, dyn_img_layout.stride_bytes}};
}

/** \brief Creates a statically typed `MutableImageView<PixelType>` view from a dynamically typed `DynImage` instance.
 *
 * Precondition: The supplied image `dyn_img` must be valid, i.e. `dyn_img.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the `DynImage` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * As the resulting `MutableImageView<PixelType>` is a non-owning view, the lifetime of the supplied `DynImage`
 * instance must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param dyn_img The dynamically typed image.
 * @return A `MutableImageView<PixelType>` instance.
 */
template <typename PixelType>
MutableImageView<PixelType> to_image_view(DynImage& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img.view());

  return MutableImageView<PixelType>{dyn_img.byte_ptr(),
                                     TypedLayout{dyn_img.width(), dyn_img.height(), dyn_img.stride_bytes()}};
}

/** \brief Creates a statically typed `ConstantImageView<PixelType>` view from a dynamically typed `DynImage` instance.
 *
 * Precondition: The supplied image `dyn_img` must be valid, i.e. `dyn_img.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the `DynImage` instance need to be
 * compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * As the resulting `ConstantImageView<PixelType>` is a non-owning view, the lifetime of the supplied `DynImage`
 * instance must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param dyn_img The dynamically typed image.
 * @return A `ConstantImageView<PixelType>` instance.
 */
template <typename PixelType>
ConstantImageView<PixelType> to_image_view(const DynImage& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img.view());

  return ConstantImageView<PixelType>{dyn_img.byte_ptr(),
                                      TypedLayout{dyn_img.width(), dyn_img.height(), dyn_img.stride_bytes()}};
}

/** \brief Creates a statically typed `ImageView<PixelType, modifiability>` view from a dynamically typed
 * `DynImageView<modifiability>` instance.
 *
 * Precondition: The supplied image view `dyn_img` must be valid, i.e. `dyn_img.is_valid()` must return true. Otherwise
 * this function will throw a `std::runtime_error` exception.
 *
 * The number of channels, the number of bytes per channel, and the sample format of the `DynImageView<>` instance need
 * to be compatible with the `PixelTraits` of `PixelType`. If this is not the case, this function will throw a
 * `std::runtime_error` exception.
 *
 * As the resulting `ImageView<PixelType, modifiability>` is a non-owning view, the lifetime of the supplied
 * `DynImageView<modifiability>` instance must exceed the lifetime of the returned instance.
 *
 * @tparam PixelType The pixel type of the `Image<PixelType>` instance to be returned.
 * @param dyn_img_view The dynamically typed image view.
 * @return A `ImageView<PixelType, modifiability>` instance.
 */
template <typename PixelType, ImageModifiability modifiability>
ImageView<PixelType, modifiability> to_image_view(const DynImageView<modifiability>& dyn_img_view)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img_view);

  return ImageView<PixelType, modifiability>{dyn_img_view.byte_ptr(),
                                             TypedLayout{dyn_img_view.width(), dyn_img_view.height(),
                                                         dyn_img_view.stride_bytes()}};
}

}  // namespace sln

#endif  // SELENE_IMG_INTEROP_DYN_IMAGE_TO_IMAGE_HPP
