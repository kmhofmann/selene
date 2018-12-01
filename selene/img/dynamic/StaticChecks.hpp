// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_REQUIREMENTS_CHECK_HPP
#define SELENE_IMG_DYNAMIC_REQUIREMENTS_CHECK_HPP

/// @file

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <type_traits>

namespace sln {

template <typename DynImageOrView>
void static_check_is_dyn_image_or_view(const DynImageOrView&)
{
  static_assert(std::is_same_v<DynImageOrView, DynImage>
                || std::is_same_v<DynImageOrView, DynImageView<ImageModifiability::Mutable>>
                || std::is_same_v<DynImageOrView, DynImageView<ImageModifiability::Constant>>,
                "Supplied type needs be either a sln::DynImage or a sln::DynImageView<>.");
}

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_REQUIREMENTS_CHECK_HPP
