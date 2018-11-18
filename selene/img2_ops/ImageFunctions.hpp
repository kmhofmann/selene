// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_FUNCTIONS_HPP
#define SELENE_IMG_IMAGE_FUNCTIONS_HPP

/// @file

//#include <selene/img2/ImageView.hpp>
#include <selene/img2/ImageBase.hpp>

namespace sln {

template <typename PixelType, typename Derived>
void fill(ImageBase<Derived>& img, PixelType value)
{
  for (PixelIndex y = 0_idx; y < img.height(); ++y)
  {
    std::fill(img.data(y), img.data_row_end(y), value);
  }
}

//template <typename PixelType>
//void fill(MutableImageView<PixelType>& img, PixelType value)
//{
//  for (PixelIndex y = 0_idx; y < img.height(); ++y)
//  {
//    std::fill(img.data(y), img.data_row_end(y), value);
//  }
//}

//template <typename PixelType>
//void fill(Image<PixelType>& img, PixelType value)
//{
//  sln::fill(img.view(), value);
//}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_FUNCTIONS_HPP
