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

namespace sln {

namespace detail {

class PixelIndexTag;

}  // namespace detail

// Use 32-bit values as indices for data access, for now.
using PixelIndex =
    ExplicitType<std::uint32_t, detail::PixelIndexTag>;  ///< Type representing an unsigned integral scalar as part of
                                                         ///< an image coordinate (x or y).

// This is the data access type for one of the accessors.
using SignedPixelIndex = std::int32_t;  ///< Type representing a signed integral scalar as part of an image coordinate
                                        ///< (x or y), when accessing image pixels via `ImageAccessor` functions.

using PixelLength = PixelIndex;  ///< Type representing a length in x or y-direction.
using Stride = Bytes;  ///< Type representing an image stride (nr of bytes per row).

inline namespace literals {

/** \brief User-defined literal representing a pixel index
 *
 * @param index Pixel index.
 * @return A `PixelIndex` instance.
 */
constexpr inline PixelIndex operator"" _px(unsigned long long index)
{
  return PixelIndex(static_cast<PixelIndex::value_type>(index));
}

}  // namespace literals

}  // namespace sln

#endif  // SELENE_IMG_TYPES_HPP
