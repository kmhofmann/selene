// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMPL_IMAGE_BASE_TRAITS_HPP
#define SELENE_IMG_TYPED_IMPL_IMAGE_BASE_TRAITS_HPP

/// @file

#include <selene/img/common/DataPtr.hpp>

#include <selene/img/typed/_impl/ImageFwd.hpp>

namespace sln::impl {

template <typename Derived>
struct ImageBaseTraits;

template <typename PixelType_, typename Allocator_>
struct ImageBaseTraits<Image<PixelType_, Allocator_>>
{
  using PixelType = PixelType_;

  constexpr static bool is_view = false;
  constexpr static bool is_modifiable = true;

  constexpr static ImageModifiability modifiability()
  {
    return ImageModifiability::Mutable;
  }
};

template <typename PixelType_, ImageModifiability modifiability_>
struct ImageBaseTraits<ImageView<PixelType_, modifiability_>>
{
  using PixelType = PixelType_;

  constexpr static bool is_view = true;
  constexpr static bool is_modifiable = (modifiability_ == ImageModifiability::Mutable);

  constexpr static ImageModifiability modifiability()
  {
    return modifiability_;
  }
};

}  // namespace sln::impl

#endif  // SELENE_IMG_TYPED_IMPL_IMAGE_BASE_TRAITS_HPP
