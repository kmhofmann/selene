// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_ALGORITHMS_HPP
#define SELENE_IMG_OPS_ALGORITHMS_HPP

/// @file

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>

#include <selene/img_ops/Allocate.hpp>
#include <selene/img_ops/_impl/TransformExpr.hpp>
#include <selene/img_ops/_impl/TransformWithPositionExpr.hpp>

#include <functional>
#include <type_traits>

namespace sln {

/// \addtogroup group-img-ops
/// @{

/** \brief Apply a function to each pixel element of an image.
 *
 * Each pixel element in the image is overwritten with the result of the function application.
 *
 * The supplied function receives a reference to the respective pixel element as first (and only) parameter.
 * Its return type, if non-void, will be ignored.
 *
 * @tparam DerivedSrc The typed image type.
 * @tparam Function The function type.
 * @param[in,out] img The image to apply the function on.
 * @param func The function to apply to each pixel element in-place.
 *             Its signature should be `void f(PixelType&)`, or any compatible callable.
 * @return `std::move(func)`.
 */
template <typename DerivedSrc, typename Function>
Function for_each_pixel(ImageBase<DerivedSrc>& img, Function func)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;
  static_assert(std::is_invocable_v<Function, PixelType&>,
                "Callable supplied to for_each_pixel must be of (or convertible to) type 'void(PixelType&)'.");

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto ptr = img.data(y), end = img.data_row_end(y); ptr != end; ++ptr)
    {
      std::invoke(func, *ptr);
    }
  }

  return std::move(func);
}

/** \brief Apply a function to each pixel element of an image.
 *
 * Each pixel element in the image is overwritten with the result of the function application.
 *
 * The supplied function receives a reference to the respective pixel element as first parameter, followed by both the
 * x and y pixel coordinates for the respective invocation.
 * Its return type, if non-void, will be ignored.
 *
 * @tparam DerivedSrc The typed image type.
 * @tparam Function The function type.
 * @param[in,out] img The image to apply the function on.
 * @param func The function to apply to each pixel element in-place.
 *             Its signature should be `void f(PixelType&, PixelIndex, PixelIndex)`, or any compatible callable.
 * @return `std::move(func)`.
 */
template <typename DerivedSrc, typename Function>
Function for_each_pixel_with_position(ImageBase<DerivedSrc>& img, Function func)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;
  static_assert(std::is_invocable_v<Function, PixelType&, PixelIndex, PixelIndex>,
                "Callable supplied to for_each_pixel_with_position must be of (or convertible to) type 'void(PixelType&, PixelIndex, PixelIndex)'.");

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    auto x = 0_idx;
    for (auto ptr = img.data(y), end = img.data_row_end(y); ptr != end; ++ptr, ++x)
    {
      std::invoke(func, *ptr, x, y);
    }
  }

  return std::move(func);
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first (and only)
 * parameter.
 * Its return type shall be of the type of a pixel element of the destination image.
 *
 * `allocate` is called on the destination image prior to performing the operation; i.e. it may be that a memory
 * allocation will take place.
 *
 * @tparam DerivedDst The typed destination image type.
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img_src The source image.
 * @param[out] img_dst The destination image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst f(PixelTypeSrc)`.
 */
template <typename DerivedDst, typename DerivedSrc, typename Function>
void transform_pixels(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, Function func)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeDst = typename ImageBase<DerivedDst>::PixelType;
  static_assert(std::is_invocable_r_v<PixelTypeDst, Function, PixelTypeSrc&>,
                "Callable supplied to transform_pixels must be of (or convertible to) type 'PixelTypeDst f(const PixelTypeSrc&)'.");

  allocate(img_dst, img_src.layout());

  for (auto y = 0_idx; y < img_dst.height(); ++y)
  {
    auto ptr_src = img_src.data(y);
    for (auto ptr_dst = img_dst.data(y), ptr_dst_end = img_dst.data_row_end(y); ptr_dst != ptr_dst_end;)
    {
      *ptr_dst++ = std::invoke(func, *ptr_src++);
    }
  }
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first (and only)
 * parameter.
 * Its return type shall be of the type of a pixel element of the destination image, as specified by the user via
 * the first template parameter.
 *
 * @tparam PixelTypeDst The pixel type of the destination image.
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img_src The source image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst f(PixelTypeSrc)`.
 * @return The destination image.
 */
template <typename PixelTypeDst, typename DerivedSrc, typename Function>
Image<PixelTypeDst> transform_pixels(const ImageBase<DerivedSrc>& img, Function func)
{
  Image<PixelTypeDst> img_dst({img.width(), img.height()});
  transform_pixels(img, img_dst, func);
  return img_dst;
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first parameter,
 * followed by both the x and y pixel coordinates for the respective invocation.
 * Its return type shall be of the type of a pixel element of the destination image.
 *
 * `allocate` is called on the destination image prior to performing the operation; i.e. it may be that a memory
 * allocation will take place.
 *
 * @tparam DerivedDst The typed destination image type.
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img_src The source image.
 * @param[out] img_dst The destination image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&, PixelIndex, PixelIndex)` or `PixelTypeDst f(PixelTypeSrc, PixelIndex, PixelIndex)`.
 */
template <typename DerivedDst, typename DerivedSrc, typename Function>
void transform_pixels_with_position(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, Function func)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeDst = typename ImageBase<DerivedDst>::PixelType;
  static_assert(std::is_invocable_r_v<PixelTypeDst, Function, PixelTypeSrc&, PixelIndex, PixelIndex>,
                "Callable supplied to transform_pixels_with_position must be of (or convertible to) type 'PixelTypeDst f(const PixelTypeSrc&, PixelIndex, PixelIndex)'.");

  allocate(img_dst, img_src.layout());

  for (auto y = 0_idx; y < img_dst.height(); ++y)
  {
    auto x = 0_idx;
    auto ptr_src = img_src.data(y);
    for (auto ptr_dst = img_dst.data(y), ptr_dst_end = img_dst.data_row_end(y); ptr_dst != ptr_dst_end;)
    {
      *ptr_dst++ = std::invoke(func, *ptr_src++, x++, y);
    }
  }
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first parameter,
 * followed by both the x and y pixel coordinates for the respective invocation.
 * Its return type shall be of the type of a pixel element of the destination image, as specified by the user via
 * the first template parameter.
 *
 * @tparam PixelTypeDst The pixel type of the destination image.
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img_src The source image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&, PixelIndex, PixelIndex)` or `PixelTypeDst f(PixelTypeSrc, PixelIndex, PixelIndex)`.
 * @return The destination image.
 */
template <typename PixelTypeDst, typename DerivedSrc, typename Function>
Image<PixelTypeDst> transform_pixels_with_position(const ImageBase<DerivedSrc>& img, Function func)
{
  Image<PixelTypeDst> img_dst({img.width(), img.height()});
  transform_pixels_with_position(img, img_dst, func);
  return img_dst;
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * This function returns an expression that is convertible to the transformed image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first (and only)
 * parameter.
 * Its return type shall be of the type of a pixel element of the destination image.
 *
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img The source image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&)` or `PixelTypeDst f(PixelTypeSrc)`.
 * @return An expression that is convertible to the transformed image.
 */
template <typename DerivedSrc, typename Function>
auto transform_pixels_expr(const ImageExpr<DerivedSrc>& img, Function func)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  static_assert(std::is_invocable_v<Function, PixelTypeSrc&>,
                "Callable supplied to transform_pixels_expr must be of (or convertible to) type 'PixelTypeDst f(const PixelTypeSrc&)'.");

  return impl::TransformExpr<ImageExpr<DerivedSrc>, Function>(img, func);
}

/** \brief Transform one image into another by applying a function to each pixel element.
 *
 * This function returns an expression that is convertible to the transformed image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * The supplied function receives a constant reference (or value) to the respective pixel element as first parameter,
 * followed by both the x and y pixel coordinates for the respective invocation.
 * Its return type shall be of the type of a pixel element of the destination image.
 *
 * @tparam DerivedSrc The typed source image type.
 * @tparam Function The function type.
 * @param img The source image.
 * @param func The function to apply to each pixel element.
 *             Its signature should be `PixelTypeDst f(const PixelTypeSrc&, PixelIndex, PixelIndex)` or `PixelTypeDst f(PixelTypeSrc, PixelIndex, PixelIndex)`.
 * @return An expression that is convertible to the transformed image.
 */
template <typename DerivedSrc, typename Function>
auto transform_pixels_with_position_expr(const ImageExpr<DerivedSrc>& img, Function func)
{
  using PixelTypeSrc = typename ImageBase<DerivedSrc>::PixelType;
  static_assert(std::is_invocable_v<Function, PixelTypeSrc&, PixelIndex, PixelIndex>,
                "Callable supplied to transform_pixels_with_position_expr must be of (or convertible to) type 'PixelTypeDst f(const PixelTypeSrc&, PixelIndex, PixelIndex)'.");

  return impl::TransformWithPositionExpr<ImageExpr<DerivedSrc>, Function>(img, func);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_ALGORITHMS_HPP
