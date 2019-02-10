// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_IMPL_IMAGE_FWD_HPP
#define SELENE_IMG_TYPED_IMPL_IMAGE_FWD_HPP

#include <selene/img/common/DataPtr.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

template <typename PixelType_, ImageModifiability modifiability_>
class ImageView;

template <typename PixelType_, typename Allocator_ = default_bytes_allocator>
class Image;

}  // namespace sln

#endif  // SELENE_IMG_TYPED_IMPL_IMAGE_FWD_HPP
