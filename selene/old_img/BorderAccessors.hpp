// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_BORDER_ACCESSORS_HPP
#define SELENE_IMG_BORDER_ACCESSORS_HPP

/// @file

#include <selene/old_img/Image.hpp>
#include <selene/old_img/Pixel.hpp>
#include <selene/old_img/PixelTraits.hpp>
#include <selene/old_img/RelativeAccessor.hpp>

namespace sln {

/** \brief The image border access mode.
 *
 * Describes the behavior when pixel values outside of the image extents are accessed.
 */
enum class BorderAccessMode
{
  Unchecked,  ///< No additional checks are performed.
  ZeroPadding,  ///< Access outside of the image extents always returns 0.
  Replicated,  ///< Access outside of the image extents is projected to the border, and the respective value is
               ///< returned.
};

/** \brief Image border accessor structure; provides a static `access` function to access image pixels according to the
 * specified border access mode.
 *
 * @tparam BorderAccessMode The border access mode to use.
 */
template <BorderAccessMode>
struct ImageBorderAccessor;

/** \brief `ImageBorderAccessor` specialization for `BorderAccessMode::Unchecked`.
 */
template <>
struct ImageBorderAccessor<BorderAccessMode::Unchecked>
{
  template <typename PixelType>
  static decltype(auto) access(const Image<PixelType>& img, PixelIndex x, PixelIndex y) noexcept;

  template <typename PixelType>
  static decltype(auto) access(const RelativeAccessor<PixelType>& img, PixelIndex rx, PixelIndex ry) noexcept;
};

/** \brief `ImageBorderAccessor` specialization for `BorderAccessMode::ZeroPadding`.
 */
template <>
struct ImageBorderAccessor<BorderAccessMode::ZeroPadding>
{
  template <typename PixelType>
  static auto access(const Image<PixelType>& img, PixelIndex x, PixelIndex y) noexcept;

  template <typename PixelType>
  static auto access(const RelativeAccessor<PixelType>& img, PixelIndex rx, PixelIndex ry) noexcept;
};

/** \brief `ImageBorderAccessor` specialization for `BorderAccessMode::Replicated`.
 */
template <>
struct ImageBorderAccessor<BorderAccessMode::Replicated>
{
  template <typename PixelType>
  static decltype(auto) access(const Image<PixelType>& img, PixelIndex x, PixelIndex y) noexcept;

  template <typename PixelType>
  static decltype(auto) access(const RelativeAccessor<PixelType>& img, PixelIndex rx, PixelIndex ry) noexcept;
};


// ----------
// Implementation:

/** \brief Accesses the pixel value of `img` at location (x, y) using the border access mode
 * `BorderAccessMode::Unchecked`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `BorderAccessMode::Unchecked`.
 */
template <typename PixelType>
inline decltype(auto)
ImageBorderAccessor<BorderAccessMode::Unchecked>::access(const Image<PixelType>& img,
                                                         PixelIndex x,
                                                         PixelIndex y) noexcept
{
  return img(to_pixel_index(x), to_pixel_index(y));
}

/** \brief Accesses the pixel value of `img` at relative location (rx, ry) using the border access mode
 * `BorderAccessMode::Unchecked`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param rx The relative x-coordinate.
 * @param ry The relative y-coordinate.
 * @return The pixel value at relative location (x, y), using `BorderAccessMode::Unchecked`.
 */
template <typename PixelType>
inline decltype(auto)
ImageBorderAccessor<BorderAccessMode::Unchecked>::access(const RelativeAccessor<PixelType>& img,
                                                         PixelIndex rx,
                                                         PixelIndex ry) noexcept
{
  const auto abs_xy = img.absolute_coordinates(rx, ry);
  return ImageBorderAccessor<BorderAccessMode::Unchecked>::access(img.image(), abs_xy.x, abs_xy.y);
}

/** \brief Accesses the pixel value of `img` at location (x, y) using the border access mode
 * `BorderAccessMode::ZeroPadding`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `BorderAccessMode::ZeroPadding`.
 */
template <typename PixelType>
inline auto
ImageBorderAccessor<BorderAccessMode::ZeroPadding>::access(const Image<PixelType>& img,
                                                           PixelIndex x,
                                                           PixelIndex y) noexcept
{
  if (x < 0 || x >= static_cast<PixelIndex>(img.width()) || y < 0
      || y >= static_cast<PixelIndex>(img.height()))
  {
    return PixelTraits<PixelType>::zero_element;
  }

  return img(to_pixel_index(x), to_pixel_index(y));
}

/** \brief Accesses the pixel value of `img` at relative location (rx, ry) using the border access mode
 * `BorderAccessMode::ZeroPadding`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param rx The relative x-coordinate.
 * @param ry The relative y-coordinate.
 * @return The pixel value at relative location (x, y), using `BorderAccessMode::ZeroPadding`.
 */
template <typename PixelType>
inline auto
ImageBorderAccessor<BorderAccessMode::ZeroPadding>::access(const RelativeAccessor<PixelType>& img,
                                                           PixelIndex rx,
                                                           PixelIndex ry) noexcept
{
  const auto abs_xy = img.absolute_coordinates(rx, ry);
  return ImageBorderAccessor<BorderAccessMode::ZeroPadding>::access(img.image(), abs_xy.x, abs_xy.y);
}

/** \brief Accesses the pixel value of `img` at location (x, y) using the border access mode
 * `BorderAccessMode::Replicated`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `BorderAccessMode::Replicated`.
 */
template <typename PixelType>
inline decltype(auto)
ImageBorderAccessor<BorderAccessMode::Replicated>::access(const Image<PixelType>& img,
                                                          PixelIndex x,
                                                          PixelIndex y) noexcept
{
  if (x < 0)
  {
    x = 0_idx;
  }
  else if (x >= static_cast<PixelIndex>(img.width()))
  {
    x = static_cast<PixelIndex>(img.width() - 1);
  }

  if (y < 0)
  {
    y = 0_idx;
  }
  else if (y >= static_cast<PixelIndex>(img.height()))
  {
    y = static_cast<PixelIndex>(img.height() - 1);
  }

  return img(to_pixel_index(x), to_pixel_index(y));
}

/** \brief Accesses the pixel value of `img` at relative location (rx, ry) using the border access mode
 * `BorderAccessMode::Replicated`.
 *
 * @tparam PixelType The pixel type.
 * @param img The image to access.
 * @param rx The relative x-coordinate.
 * @param ry The relative y-coordinate.
 * @return The pixel value at relative location (x, y), using `BorderAccessMode::Replicated`.
 */
template <typename PixelType>
inline decltype(auto)
ImageBorderAccessor<BorderAccessMode::Replicated>::access(const RelativeAccessor<PixelType>& img,
                                                          PixelIndex rx,
                                                          PixelIndex ry) noexcept
{
  const auto abs_xy = img.absolute_coordinates(rx, ry);
  return ImageBorderAccessor<BorderAccessMode::Replicated>::access(img.image(), abs_xy.x, abs_xy.y);
}

}  // namespace sln

#endif  // SELENE_IMG_BORDER_ACCESSORS_HPP
