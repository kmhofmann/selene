// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_IMPL_UTILS_HPP
#define SELENE_IMG_DYNAMIC_IMPL_UTILS_HPP

/// @file

#include <selene/img/dynamic/_impl/RuntimeChecks.hpp>
#include <selene/img/dynamic/_impl/StaticChecks.hpp>

namespace sln {
namespace impl {

template <typename DynImageOrView>
[[nodiscard]] inline
bool prepare_image_or_view(DynImageOrView& dyn_img_or_view, const UntypedLayout& layout, const UntypedImageSemantics& semantics)
{
  impl::static_check_is_dyn_image_or_mutable_view<DynImageOrView>();

  if constexpr (impl::static_check_is_dyn_image<DynImageOrView>())
  {
    dyn_img_or_view.reallocate(layout, ImageRowAlignment{0}, semantics, false);
  }
  else
  {
    const auto compatible = impl::check_is_layout_compatible(dyn_img_or_view, layout);

    if (!compatible)
    {
      return false;
    }

    // Pointer and layout cannot change, but the semantics might.
    dyn_img_or_view = MutableDynImageView(dyn_img_or_view.byte_ptr(), dyn_img_or_view.layout(), semantics);
  }

  return true;
}

}  // namespace impl
}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_IMPL_UTILS_HPP
