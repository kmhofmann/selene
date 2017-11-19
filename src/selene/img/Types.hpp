// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPES_HPP
#define SELENE_IMG_TYPES_HPP

#include <cstdint>

namespace selene {
namespace img {

// use 32-bit values as indices for data access, for now
using Index = std::uint32_t;
using Length = std::uint32_t;
using Stride = std::uint32_t;

} // namespace img
} // namespace selene

#endif // SELENE_IMG_TYPES_HPP