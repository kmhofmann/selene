// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_ALLOCATE_HPP
#define SELENE_IMG_OPS_ALLOCATE_HPP

/// @file

#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/TypedLayout.hpp>

#include <cstdint>
#include <stdexcept>

namespace sln {

/// \addtogroup group-img-ops
/// @{

template <typename Derived>
void allocate(ImageBase<Derived>& img_dst,
              TypedLayout layout,
              bool force_layout = false)
{
  if (!force_layout && img_dst.width() == layout.width && img_dst.height() == layout.height)
  {
    return;
  }

  if constexpr(ImageBase<Derived>::is_view)
  {
    throw std::runtime_error("Cannot resize image view.");
  }
  else
  {
    img_dst.derived().reallocate(layout);
  }
}

/// @}

}  // namespace sln

#endif  // SELENE_IMG_OPS_ALLOCATE_HPP
