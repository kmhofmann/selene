// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
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

namespace impl {

class PixelIndexTag;
class PixelLengthTag;
class StrideTag;
class ImageRowAlignmentTag;

}  // namespace impl

using PixelIndex = ExplicitType<std::int32_t, impl::PixelIndexTag>;  ///< Type representing a signed integral
                                                                       ///< scalar as part of an image coordinate
                                                                       ///< (x or y).

using PixelLength = ExplicitType<std::int32_t, impl::PixelLengthTag>;  ///< Type representing a length in
                                                                         ///< x or y-direction.

using Stride = ExplicitType<std::size_t, impl::StrideTag>;  ///< Type representing an image stride
                                                              ///< (nr of bytes per row).

using ImageRowAlignment = ExplicitType<std::size_t, impl::ImageRowAlignmentTag>;  ///< Type representing an image row
                                                                                    ///< alignment.

/** \brief Explicitly converts the provided value to `PixelIndex` type.
 *
 * This operation should usually be optimized away, but provides stronger type safety.
 *
 * @tparam T The value type.
 * @param value The value.
 * @return The value as `PixelIndex`.
 */
template <typename T>
constexpr inline PixelIndex to_pixel_index(T value)
{
  return PixelIndex{static_cast<PixelIndex::value_type>(value)};
}

/** \brief Explicitly converts the provided value to `PixelLength` type.
 *
 * This operation should usually be optimized away, but provides stronger type safety.
 *
 * @tparam T The value type.
 * @param value The value.
 * @return The value as `PixelLength`.
 */
template <typename T>
constexpr inline PixelLength to_pixel_length(T value)
{
  return PixelLength{static_cast<PixelLength::value_type>(value)};
}

inline namespace literals {

 /** \brief User-defined literal representing a pixel index
 *
 * @param index Pixel index.
 * @return A `PixelIndex` instance.
 */
constexpr inline PixelIndex operator"" _idx(unsigned long long index)
{
  return to_pixel_index(index);
}

/** \brief User-defined literal representing a pixel length
 *
 * @param length Pixel length.
 * @return A `PixelLength` instance.
 */
constexpr inline PixelLength operator"" _px(unsigned long long length)
{
  return to_pixel_length(length);
}

}  // namespace literals

// ----------

namespace impl {

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

}  // namespace impl

}  // namespace sln

#endif  // SELENE_IMG_TYPES_HPP
