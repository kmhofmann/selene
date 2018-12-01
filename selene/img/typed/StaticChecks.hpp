// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_STATIC_CHECKS_HPP
#define SELENE_IMG_TYPED_STATIC_CHECKS_HPP

/// @file

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <type_traits>

namespace sln {

template <typename ImageOrView>
void static_check_is_image_or_view(const ImageOrView&)
{
  static_assert(std::is_same_v<ImageOrView, DynImage>
                || std::is_same_v<ImageOrView, ImageView<ImageModifiability::Mutable>>
                || std::is_same_v<ImageOrView, ImageView<ImageModifiability::Constant>>,
                "Supplied type needs be either a sln::Image or a sln::ImageView<>.");
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED_STATIC_CHECKS_HPP
