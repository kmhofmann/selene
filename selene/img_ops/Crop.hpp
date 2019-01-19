// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_CROP_HPP
#define SELENE_IMG_OPS_CROP_HPP

/// @file

#include <selene/img/common/BoundingBox.hpp>

#include <selene/img/typed/ImageBase.hpp>

#include <selene/img_ops/Clone.hpp>

#include <utility>

namespace sln {

/// \addtogroup group-img-ops
/// @{

template <typename DerivedSrcDst>
void crop(ImageBase<DerivedSrcDst>& img, const BoundingBox& region)
{
  auto cropped_clone = clone(img, region);
  img = std::move(cropped_clone);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_CROP_HPP
