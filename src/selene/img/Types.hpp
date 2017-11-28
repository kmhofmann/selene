// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPES_HPP
#define SELENE_IMG_TYPES_HPP

/// @file

#include <cstdint>

namespace selene {
namespace img {

// use 32-bit values as indices for data access, for now
using Index = std::uint32_t;  ///< Type representing a scalar as part of an image coordinate (x or y).
using Length = std::uint32_t;  ///< Type representing a length in x or y-direction.
using Stride = std::uint32_t;  ///< Type representing an image stride (nr of bytes per row).

} // namespace img
} // namespace selene

#endif // SELENE_IMG_TYPES_HPP
