// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_DYNAMIC_IMPL_DYN_IMAGE_FWD_HPP
#define SELENE_IMG_DYNAMIC_IMPL_DYN_IMAGE_FWD_HPP

#include <selene/img/common/DataPtr.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

template <ImageModifiability modifiability_ = ImageModifiability::Constant>
class DynImageView;

template <typename Allocator_ = default_bytes_allocator>
class DynImage;

}  // namespace sln

#endif  // SELENE_IMG_DYNAMIC_IMPL_DYN_IMAGE_FWD_HPP
