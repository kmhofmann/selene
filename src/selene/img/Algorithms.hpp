// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_ALGORITHMS_HPP
#define SELENE_IMG_ALGORITHMS_HPP

/// @file

#include <selene/img/Image.hpp>
#include <selene/img/Pixel.hpp>

namespace sln {

template <typename PixelType, typename UnaryFunction>
UnaryFunction for_each_pixel(Image<PixelType>& img, UnaryFunction f)
{
  for (auto y = 0_px; y < img.height(); ++y)
  {
    for (auto ptr = img.data(y), end = img.data_row_end(y); ptr != end; ++ptr)
    {
      f(*ptr);
    }
  }

  return std::move(f);
}

template <typename PixelTypeDst, typename PixelTypeSrc, typename UnaryOperation>
void transform_pixels(const Image<PixelTypeSrc>& img_src, Image<PixelTypeDst>& img_dst, UnaryOperation op)
{
  img_dst.maybe_allocate(img_src.width(), img_src.height());

  for (auto y = 0_px; y < img_dst.height(); ++y)
  {
    auto ptr_src = img_src.data(y);
    for (auto ptr_dst = img_dst.data(y), ptr_dst_end = img_dst.data_row_end(y); ptr_dst != ptr_dst_end;)
    {
      *ptr_dst++ = op(*ptr_src++);
    }
  }
}

template <typename PixelTypeDst, typename PixelTypeSrc, typename UnaryOperation>
Image<PixelTypeDst> transform_pixels(const Image<PixelTypeSrc>& img_src, UnaryOperation op)
{
  Image<PixelTypeDst> img_dst(img_src.width(), img_src.height());
  transform_pixels(img_src, img_dst, op);
  return img_dst;
}

}  // namespace sln

#endif  // SELENE_IMG_ALGORITHMS_HPP
