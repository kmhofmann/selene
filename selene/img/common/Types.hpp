// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_COMMON_TYPES_HPP
#define SELENE_IMG_COMMON_TYPES_HPP

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

using PixelIndex = sln::impl::ExplicitType<std::int32_t, sln::impl::PixelIndexTag>;  ///< Type representing a signed
                                                                                     ///< integral scalar as part of an
                                                                                     ///< image coordinate (x or y).

using PixelLength = sln::impl::ExplicitType<std::int32_t, sln::impl::PixelLengthTag>;  ///< Type representing a length in
                                                                                       ///< x or y-direction.

using Stride = sln::impl::ExplicitType<std::ptrdiff_t, sln::impl::StrideTag>;  ///< Type representing an image stride
                                                                               ///< (nr of bytes per row).

using ImageRowAlignment = sln::impl::ExplicitType<std::ptrdiff_t,sln::impl::ImageRowAlignmentTag>;  ///< Type
                                                                                                    ///< representing an
                                                                                                    ///< image row
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
constexpr inline PixelIndex to_pixel_index(T value) noexcept
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
constexpr inline PixelLength to_pixel_length(T value) noexcept
{
  return PixelLength{static_cast<PixelLength::value_type>(value)};
}

inline namespace literals {

 /** \brief User-defined literal representing a pixel index
 *
 * @param index Pixel index.
 * @return A `PixelIndex` instance.
 */
constexpr inline PixelIndex operator"" _idx(unsigned long long index) noexcept
{
  return to_pixel_index(index);
}

/** \brief User-defined literal representing a pixel length
 *
 * @param length Pixel length.
 * @return A `PixelLength` instance.
 */
constexpr inline PixelLength operator"" _px(unsigned long long length) noexcept
{
  return to_pixel_length(length);
}

}  // namespace literals

// ----------

namespace impl {

inline Stride compute_stride_bytes(std::ptrdiff_t row_bytes, std::ptrdiff_t alignment_bytes)
{
  if (alignment_bytes <= 1)
  {
    return Stride{row_bytes};
  }

  const auto mod = row_bytes % alignment_bytes;

  if (mod == 0)
  {
    return Stride{row_bytes};
  }

  const auto stride_bytes = Stride{row_bytes + alignment_bytes - mod};
  SELENE_ASSERT(stride_bytes % alignment_bytes == 0);
  return stride_bytes;
}

inline ImageRowAlignment guess_row_alignment(std::uintptr_t ptr, std::ptrdiff_t stride_bytes)
{
  if (ptr % 128 == 0 && stride_bytes % 128 == 0)
  {
    return ImageRowAlignment{128};
  }

  if (ptr % 64 == 0 && stride_bytes % 64 == 0)
  {
    return ImageRowAlignment{64};
  }

  if (ptr % 32 == 0 && stride_bytes % 32 == 0)
  {
    return ImageRowAlignment{32};
  }

  if (ptr % 16 == 0 && stride_bytes % 16 == 0)
  {
    return ImageRowAlignment{16};
  }

  if (ptr % 8 == 0 && stride_bytes % 8 == 0)
  {
    return ImageRowAlignment{8};
  }

  if (ptr % 4 == 0 && stride_bytes % 4 == 0)
  {
    return ImageRowAlignment{4};
  }

  if (ptr % 2 == 0 && stride_bytes % 2 == 0)
  {
    return ImageRowAlignment{2};
  }

  return ImageRowAlignment{1};
}

}  // namespace impl

}  // namespace sln

#endif  // SELENE_IMG_COMMON_TYPES_HPP
