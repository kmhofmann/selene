// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_COMMON_TYPES_HPP
#define SELENE_IMG_COMMON_TYPES_HPP

/// @file

#include <selene/base/Assert.hpp>
#include <selene/base/Bitcount.hpp>
#include <selene/base/Types.hpp>

#include <cstdint>
#include <limits>
#include <memory>

namespace sln {

/// \addtogroup group-img-common
/// @{

namespace impl {

class PixelIndexTag;
class PixelLengthTag;
class StrideTag;

}  // namespace impl

using PixelIndex = sln::impl::ExplicitType<std::int32_t, sln::impl::PixelIndexTag>;  ///< Type representing a signed
                                                                                     ///< integral scalar as part of an
                                                                                     ///< image coordinate (x or y).

using PixelLength = sln::impl::ExplicitType<std::int32_t, sln::impl::PixelLengthTag>;  ///< Type representing a length in
                                                                                       ///< x or y-direction.

using Stride = sln::impl::ExplicitType<std::ptrdiff_t, sln::impl::StrideTag>;  ///< Type representing an image stride
                                                                               ///< (nr of bytes per row).

using default_bytes_allocator = std::allocator<std::uint8_t>;

/** \brief Explicitly converts the provided value to `PixelIndex` type.
 *
 * This operation should usually be optimized away, but provides stronger type safety.
 *
 * @tparam T The value type.
 * @param value The value.
 * @return The value as `PixelIndex`.
 */
template <typename T>
constexpr PixelIndex to_pixel_index(T value) noexcept
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
constexpr PixelLength to_pixel_length(T value) noexcept
{
  return PixelLength{static_cast<PixelLength::value_type>(value)};
}

/** \brief Explicitly converts the provided value to `Stride` type.
 *
 * This operation should usually be optimized away, but provides stronger type safety.
 *
 * @tparam T The value type.
 * @param value The value.
 * @return The value as `Stride`.
 */
template <typename T>
constexpr Stride to_stride(T value) noexcept
{
  return Stride{static_cast<Stride::value_type>(value)};
}

inline namespace literals {

 /** \brief User-defined literal representing a pixel index
 *
 * @param index Pixel index.
 * @return A `PixelIndex` instance.
 */
constexpr PixelIndex operator"" _idx(unsigned long long index) noexcept
{
  return to_pixel_index(index);
}

/** \brief User-defined literal representing a pixel length
 *
 * @param length Pixel length.
 * @return A `PixelLength` instance.
 */
constexpr PixelLength operator"" _px(unsigned long long length) noexcept
{
  return to_pixel_length(length);
}

}  // namespace literals

/// @}

}  // namespace sln

#endif  // SELENE_IMG_COMMON_TYPES_HPP
