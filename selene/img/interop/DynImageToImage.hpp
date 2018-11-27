// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
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

template <typename PixelType>
Image<PixelType> to_image(DynImage&& dyn_img);

template <typename PixelType>
MutableImageView<PixelType> to_image_view(DynImage& dyn_img);

template <typename PixelType, ImageModifiability modifiability>
ImageView<PixelType, modifiability> to_image_view(const DynImageView<modifiability>& dyn_img);

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
      && pixel_format != PixelFormat::Unknown  // TODO: should conversion to Unknown be allowed?
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

// TODO: adapt documentation from old function
template <typename PixelType>
Image<PixelType> to_image(DynImage&& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img.view());

  const auto dyn_img_layout = dyn_img.layout();
  auto memory = dyn_img.relinquish_data_ownership();
  return Image<PixelType>{std::move(memory),
                          TypedLayout{dyn_img_layout.width, dyn_img_layout.height, dyn_img_layout.stride_bytes}};
}

// TODO: adapt documentation from old function
template <typename PixelType>
MutableImageView<PixelType> to_image_view(DynImage& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img.view());

  return MutableImageView<PixelType>{dyn_img.byte_ptr(),
                                     TypedLayout{dyn_img.width(), dyn_img.height(), dyn_img.stride_bytes()}};
}

// TODO: adapt documentation from old function
template <typename PixelType, ImageModifiability modifiability>
ImageView<PixelType, modifiability> to_image_view(const DynImageView<modifiability>& dyn_img)
{
  impl::check_dyn_img_to_img_compatibility<PixelType>(dyn_img);

  return ImageView<PixelType, modifiability>{dyn_img.byte_ptr(),
                                             TypedLayout{dyn_img.width(), dyn_img.height(), dyn_img.stride_bytes()}};
}

}  // namespace sln

#endif  // SELENE_IMG_INTEROP_DYN_IMAGE_TO_IMAGE_HPP
