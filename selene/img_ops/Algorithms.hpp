// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_ALGORITHMS_HPP
#define SELENE_IMG_OPS_ALGORITHMS_HPP

/// @file

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>

#include <selene/img_ops/Allocate.hpp>

namespace sln {

/** \brief Applies a unary function to each pixel value of an image.
 *
 * @tparam DerivedSrc The typed image type.
 * @tparam UnaryFunction The unary function type.
 * @param[in,out] img The image to apply the function on.
 * @param f The unary function to apply to each pixel. Its signature should be `void f(PixelType&)`.
 * @return `std::move(f)`.
 */
template <typename DerivedSrc, typename UnaryFunction>
UnaryFunction for_each_pixel(ImageBase<DerivedSrc>& img, UnaryFunction f)
{
  for (auto y = 0_idx; y < img.height(); ++y)
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
 * `allocate` is called on the destination image prior to performing the operation.
 *
 * @tparam DerivedDst The typed destination image type.
 * @tparam DerivedSrc The typed source image type.
 * @tparam UnaryOperation The unary operation type.
 * @param img_src The source image.
 * @param[out] img_dst The destination image.
 * @param op The unary operation. Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst
 * f(PixelTypeSrc)`.
 */
template <typename DerivedDst, typename DerivedSrc, typename UnaryOperation>
void transform_pixels(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, UnaryOperation op)
{
  allocate(img_dst, img_src.layout());

  for (auto y = 0_idx; y < img_dst.height(); ++y)
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
 * @tparam DerivedSrc The typed source image type.
 * @tparam UnaryOperation The unary operation type.
 * @param img_src The source image.
 * @param op The unary operation. Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst
 * f(PixelTypeSrc)`.
 * @return The destination image.
 */
template <typename PixelTypeDst, typename DerivedSrc, typename UnaryOperation>
Image<PixelTypeDst> transform_pixels(const ImageBase<DerivedSrc>& img_src, UnaryOperation op)
{
  Image<PixelTypeDst> img_dst({img_src.width(), img_src.height()});
  transform_pixels(img_src, img_dst, op);
  return img_dst;
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_ALGORITHMS_HPP
