// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_GENERATE_HPP
#define SELENE_IMG_OPS_GENERATE_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>
#include <selene/img/typed/ImageBase.hpp>

#include <selene/img_ops/_impl/GenerationExpr.hpp>

#include <type_traits>

namespace sln {

/// \addtogroup group-img-ops
/// @{

template <typename Function, typename DerivedSrc>
auto generate(const Function& func, ImageBase<DerivedSrc>& img)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;
  using PixelTypeFunc = std::invoke_result_t<Function, PixelIndex, PixelIndex>;

  static_assert(std::is_same_v<typename PixelTraits<PixelType>::Element,
                               typename PixelTraits<PixelTypeFunc>::Element>
                && PixelTraits<PixelType>::nr_channels == PixelTraits<PixelTypeFunc>::nr_channels,
                "Cannot generate image using function with incompatible pixel type");

  for (PixelIndex y = 0_idx; y < img.height(); ++y)
  {
    PixelIndex x = 0_idx;
    for (auto itr = img.data(y), itr_end = img.data_row_end(y); itr != itr_end; ++itr, ++x)
    {
      *itr = func(x, y);
    }
  }
}

template <typename Function, typename Allocator = default_bytes_allocator>
auto generate(const Function& func, PixelLength width, PixelLength height)
{
  using PixelType = std::invoke_result_t<Function, PixelIndex, PixelIndex>;

  Image<PixelType, Allocator> img({width, height});
  generate(func, img);
  return img;
}

template <typename Function>
auto generate_expr(const Function& func, PixelLength width, PixelLength height)
{
  return impl::GenerationExpr<Function>(func, width, height);
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_GENERATE_HPP
