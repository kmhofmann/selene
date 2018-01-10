// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPES_HPP
#define SELENE_IMG_TYPES_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/Bitcount.hpp>
#include <selene/base/Types.hpp>

#include <cstdint>
#include <limits>

namespace sln {

namespace detail {

class PixelIndexTag;
class StrideTag;
class ImageRowAlignmentTag;

}  // namespace detail

// Use 32-bit values as indices for data access, for now.
using PixelIndex = ExplicitType<std::uint32_t, detail::PixelIndexTag>;  ///< Type representing an unsigned integral
                                                                        ///< scalar as part of an image coordinate
                                                                        ///< (x or y).

// This is the data access type for one of the accessors.
using SignedPixelIndex = std::int32_t;  ///< Type representing a signed integral scalar as part of an image coordinate
                                        ///< (x or y), when accessing image pixels via `ImageAccessor` functions.

using PixelLength = PixelIndex;  ///< Type representing a length in x or y-direction.
using Stride = ExplicitType<std::size_t, detail::StrideTag>;  ///< Type representing an image stride
                                                              ///< (nr of bytes per row).
using ImageRowAlignment = ExplicitType<std::size_t, detail::ImageRowAlignmentTag>;  ///< Type representing an image row
                                                                                    ///< alignment.

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

// ----------

namespace detail {

inline Stride compute_stride_bytes(std::size_t row_bytes, std::size_t alignment_bytes)
{
  if (alignment_bytes == 0)
  {
    return Stride{row_bytes};
  }

  const auto stride_bytes = Stride{row_bytes + alignment_bytes - (row_bytes % alignment_bytes)};
  SELENE_ASSERT(stride_bytes % alignment_bytes == 0);
  return stride_bytes;
}

inline std::size_t guess_row_alignment(std::uintptr_t ptr, std::size_t stride_bytes, std::size_t start_alignment = 128)
{
  SELENE_ASSERT(start_alignment > 0 && bit_count(start_alignment) == 1);  // should be power of 2

  while (start_alignment > 0)
  {
    if (ptr % start_alignment == 0 && stride_bytes % start_alignment == 0)
    {
      return start_alignment;
    }

    start_alignment >>= 1;
  }

  return start_alignment;
}

}  // namespace detail

}  // namespace sln

#endif  // SELENE_IMG_TYPES_HPP
