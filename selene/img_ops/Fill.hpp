// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_FILL_HPP
#define SELENE_IMG_OPS_FILL_HPP

/// @file

#include <selene/img/typed/ImageBase.hpp>

#include <algorithm>

namespace sln {

template <typename PixelType, typename Derived>
void fill(ImageBase<Derived>& img_dst, PixelType value)
{
  for (PixelIndex y = 0_idx; y < img_dst.height(); ++y)
  {
    std::fill(img_dst.data(y), img_dst.data_row_end(y), value);
  }
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_FILL_HPP
