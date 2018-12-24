// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_TYPED_TYPED_LAYOUT_HPP
#define SELENE_IMG_TYPED_TYPED_LAYOUT_HPP

/// @file

#include <selene/base/Assert.hpp>

#include <selene/img/common/PixelFormat.hpp>
#include <selene/img/pixel/PixelTraits.hpp>
#include <selene/img/common/Types.hpp>

namespace sln {

/** \brief The layout for a statically typed image, holding information about width, height, and the image's row stride
 * in bytes.
 */
class TypedLayout
{
public:
  constexpr TypedLayout() noexcept
      : width{PixelLength{0}}
      , height{PixelLength{0}}
      , stride_bytes{Stride{0}}
  { }

  constexpr TypedLayout(PixelLength width_,
                        PixelLength height_) noexcept
      : width(width_)
      , height(height_)
      , stride_bytes(Stride{0})  // special value: to be set later
  { }

  constexpr TypedLayout(PixelLength width_,
                        PixelLength height_,
                        Stride stride_bytes_) noexcept
      : width(width_)
      , height(height_)
      , stride_bytes(stride_bytes_)
  { }

  PixelLength width;  ///< The image width in pixels.
  PixelLength height;  ///< The image height in pixels.
  Stride stride_bytes;  ///< The image row stride in bytes. The layout may include additional padding bytes.

  template <typename PixelType> constexpr std::ptrdiff_t nr_bytes_per_pixel() const noexcept;
  template <typename PixelType> constexpr std::ptrdiff_t row_bytes() const noexcept;
  template <typename PixelType> constexpr std::ptrdiff_t total_bytes() const noexcept;
  template <typename PixelType> constexpr bool is_packed() const noexcept;
};

constexpr bool operator==(const TypedLayout& l, const TypedLayout& r);

constexpr bool operator!=(const TypedLayout& l, const TypedLayout& r);

// ----------
// Implementation:

/** \brief Returns the number of bytes per pixel.
 *
 * @tparam PixelType The pixel type.
 * @return The number of bytes per pixel.
 */
template <typename PixelType> constexpr std::ptrdiff_t TypedLayout::nr_bytes_per_pixel() const noexcept
{
  return PixelTraits<PixelType>::nr_bytes;
}

/** \brief Returns the number of data bytes occupied by each image row.
 *
 *  The value returned is equal to `(width * nr_bytes_per_pixel<PixelType>())`.
 * It follows that `stride_bytes >= row_bytes()`, since `stride_bytes` may include additional padding bytes.
 *
 * @tparam PixelType The pixel type.
 * @return Number of data bytes occupied by each image row.
 */
template <typename PixelType> constexpr std::ptrdiff_t TypedLayout::row_bytes() const noexcept
{
  return width * nr_bytes_per_pixel<PixelType>();
}

/** \brief Returns the total number of bytes occupied by the image data in memory.
 *
 * @tparam PixelType The pixel type.
 * @return Number of bytes occupied by the image data in memory.
 */
template <typename PixelType> constexpr std::ptrdiff_t TypedLayout::total_bytes() const noexcept
{
  SELENE_ASSERT(stride_bytes >= PixelTraits<PixelType>::nr_bytes * width);
  return stride_bytes * height;
}

/** \brief Returns whether image data is stored packed in memory using this layout.
 *
 * @tparam PixelType The pixel type.
 * @return True, if the image data is stored packed using this layout; false otherwise.
 */
template <typename PixelType> constexpr bool TypedLayout::is_packed() const noexcept
{
  SELENE_ASSERT(stride_bytes >= PixelTraits<PixelType>::nr_bytes * width);
  return stride_bytes == PixelTraits<PixelType>::nr_bytes * width;
}

/** \brief Equality comparison for two typed layouts.
 *
 * @tparam PixelType The pixel type.
 * @param l The left-hand side layout to compare.
 * @param r The right-hand side layout to compare.
 * @return True, if the two layouts are identical; false otherwise.
 */
constexpr  bool operator==(const TypedLayout& l, const TypedLayout& r)
{
  return l.width == r.width && l.height == r.height && l.stride_bytes == r.stride_bytes;
}

/** \brief Inequality comparison for two typed layouts.
 *
 * @tparam PixelType The pixel type.
 * @param l The left-hand side layout to compare.
 * @param r The right-hand side layout to compare.
 * @return True, if the two layouts are not equal; false otherwise.
 */
constexpr  bool operator!=(const TypedLayout& l, const TypedLayout& r)
{
  return !(l == r);
}

}  // namespace sln

#endif  // SELENE_IMG_TYPED__TYPED_LAYOUT_HPP
