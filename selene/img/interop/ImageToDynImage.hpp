// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
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

template <typename PixelType, ImageModifiability modifiability>
DynImageView<modifiability> to_dyn_image_view(const ImageView<PixelType, modifiability>& img, PixelFormat new_pixel_format = PixelFormat::Invalid);

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
}

}  // namespace impl

// TODO: adapt documentation from old function
template <typename PixelType>
DynImage to_dyn_image(Image<PixelType>&& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img, new_pixel_format);

  const auto& img_layout = img.layout();

  auto memory = img.relinquish_data_ownership();
  return DynImage{std::move(memory),
                  {img_layout.width, img_layout.height, nr_channels, nr_bytes_per_channel, img_layout.stride_bytes},
                  {new_pixel_format, sample_format}};
}

// TODO: adapt documentation from old function
template <typename PixelType>
MutableDynImageView to_dyn_image_view(Image<PixelType>& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img, new_pixel_format);

  return MutableDynImageView{img.byte_ptr(),
                             UntypedLayout{img.width(), img.height(), nr_channels, nr_bytes_per_channel, img.stride_bytes()},
                             UntypedImageSemantics{new_pixel_format, sample_format}};
}

// TODO: adapt documentation from old function
template <typename PixelType, ImageModifiability modifiability>
DynImageView<modifiability> to_dyn_image_view(const ImageView<PixelType, modifiability>& img, PixelFormat new_pixel_format)
{
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;
  constexpr auto sample_format = PixelTraits<PixelType>::sample_format;

  new_pixel_format = impl::check_img_to_dyn_img_compatibility(img, new_pixel_format);

  return DynImageView<modifiability>{img.byte_ptr(),
                                     UntypedLayout{img.width(), img.height(), nr_channels, nr_bytes_per_channel, img.stride_bytes()},
                                     UntypedImageSemantics{new_pixel_format, sample_format}};
}

}  // namespace sln

#endif  // SELENE_IMG_INTEROP_IMAGE_TO_DYN_IMAGE_HPP
