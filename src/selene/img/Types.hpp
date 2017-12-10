// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPES_HPP
#define SELENE_IMG_TYPES_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/Types.hpp>

#include <cstdint>
#include <limits>

namespace selene {
namespace img {

namespace detail {

class IndexTag;

}  // namespace detail

// use 32-bit values as indices for data access, for now
using Index = ExplicitType<std::uint32_t, detail::IndexTag>;  ///< Type representing a scalar as part of an image
                                                              ///< coordinate (x or y).
using Length = Index;  ///< Type representing a length in x or y-direction.
using Stride = Bytes;  ///< Type representing an image stride (nr of bytes per row).

/** \brief User-defined literal representing a pixel index
 *
 * @param index Pixel index.
 * @return An `Index` instance.
 */
constexpr inline Index operator"" _px(unsigned long long index)
{
  return Index(static_cast<Index::value_type>(index));
}

}  // namespace img
}  // namespace selene

#endif  // SELENE_IMG_TYPES_HPP
