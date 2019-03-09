// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CROP_HPP
#define SELENE_IMG_OPS_CROP_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/typed/ImageBase.hpp>

#include <selene/img_ops/Clone.hpp>
#include <selene/img_ops/_impl/CropExpr.hpp>

#include <utility>

namespace sln {

/// \addtogroup group-img-ops
/// @{

/** \brief Crop the supplied image using the specified region.
 *
 * @tparam DerivedSrcDst The typed source & destination image type.
 * @param img The source and destination image.
 * @param region The bounding box region that designates the cropping area.
 */
template <typename DerivedSrcDst>
void crop(ImageBase<DerivedSrcDst>& img, const BoundingBox& region)
{
  auto cropped_clone = clone(img, region);
  img.derived() = std::move(cropped_clone);
}

/** \brief Crop the supplied image using the specified region.
 *
 * This function returns an expression that is convertible to the cropped image.
 * As such, it enables delayed evaluation; the evaluation shall take place at the moment of conversion.
 * Evaluation can also be triggered by calling the `eval()` member function of the return expression.
 *
 * @tparam DerivedSrcDst The typed source & destination image type.
 * @param img The source and destination image.
 * @param region The bounding box region that designates the cropping area.
 * @return An expression that is convertible to the cropped image.
 */
template <typename DerivedSrc>
auto crop_expr(const ImageExpr<DerivedSrc>& img, const BoundingBox& region)
{
  return impl::CropExpr<ImageExpr<DerivedSrc>>(img, region);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_CROP_HPP
