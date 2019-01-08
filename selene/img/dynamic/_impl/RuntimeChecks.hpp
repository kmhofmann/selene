// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_IMPL_RUNTIME_CHECKS_HPP
#define SELENE_IMG_DYNAMIC_IMPL_RUNTIME_CHECKS_HPP

/// @file

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

namespace sln {

namespace impl {

template <typename DynImageOrView>
inline bool check_is_layout_compatible(const DynImageOrView& img, const UntypedLayout& layout)
{
  // Everything has to match, except the stride.
  return img.layout().width == layout.width
      && img.layout().height == layout.height
      && img.layout().nr_channels == layout.nr_channels
      && img.layout().nr_bytes_per_channel == layout.nr_bytes_per_channel;
}

} // namespace impl

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_IMPL_RUNTIME_CHECKS_HPP
