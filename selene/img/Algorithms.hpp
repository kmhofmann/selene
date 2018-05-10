// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_ALGORITHMS_HPP
#define SELENE_IMG_ALGORITHMS_HPP

/// @file

#include <selene/img/Image.hpp>
#include <selene/img/Pixel.hpp>

namespace sln {

/** \brief Applies a unary function to each pixel value of an image.
 *
 * @tparam PixelType The pixel type of the image.
 * @tparam UnaryFunction The unary function type.
 * @param[in,out] img The image to apply the function on.
 * @param f The unary function to apply to each pixel. Its signature should be `void f(PixelType&)`.
 * @return `std::move(f)`.
 */
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

/** \brief Transforms one image into another by applying a unary operation to each pixel value.
 *
 * `sln::Image<PixelTypeDst>::maybe_allocate` is called on the destination image prior to performing the operation.
 *
 * @tparam PixelTypeDst The pixel type of the destination image.
 * @tparam PixelTypeSrc The pixel type of the source image.
 * @tparam UnaryOperation The unary operation type.
 * @param img_src The source image.
 * @param[out] img_dst The destination image.
 * @param op The unary operation. Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst
 * f(PixelTypeSrc)`.
 */
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

/** \brief Transforms one image into another by applying a unary operation to each pixel value.
 *
 * @tparam PixelTypeDst The pixel type of the destination image.
 * @tparam PixelTypeSrc The pixel type of the source image.
 * @tparam UnaryOperation The unary operation type.
 * @param img_src The source image.
 * @param op The unary operation. Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst
 * f(PixelTypeSrc)`.
 * @return The destination image.
 */
template <typename PixelTypeDst, typename PixelTypeSrc, typename UnaryOperation>
Image<PixelTypeDst> transform_pixels(const Image<PixelTypeSrc>& img_src, UnaryOperation op)
{
  Image<PixelTypeDst> img_dst(img_src.width(), img_src.height());
  transform_pixels(img_src, img_dst, op);
  return img_dst;
}

}  // namespace sln

#endif  // SELENE_IMG_ALGORITHMS_HPP
