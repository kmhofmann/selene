// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_ACCESSORS_HPP
#define SELENE_IMG_ACCESSORS_HPP

/// @file

#include <selene/img/Image.hpp>
#include <selene/img/Pixel.hpp>
#include <selene/img/PixelTraits.hpp>

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

/** \brief Image accessor structure; provides a static `access` function to access image pixels according to the
 * specified border access mode.
 *
 * @tparam BorderAccessMode The border access mode to use.
 */
template <BorderAccessMode>
struct ImageAccessor;

/** \brief `ImageAccessor` specialization for `BorderAccessMode::Unchecked`.
 */
template <>
struct ImageAccessor<BorderAccessMode::Unchecked>
{
  template <typename PixelType>
  static auto access(const Image<PixelType>& img, SignedPixelIndex x, SignedPixelIndex y);
};

/** \brief `ImageAccessor` specialization for `BorderAccessMode::ZeroPadding`.
 */
template <>
struct ImageAccessor<BorderAccessMode::ZeroPadding>
{
  template <typename PixelType>
  static auto access(const Image<PixelType>& img, SignedPixelIndex x, SignedPixelIndex y);
};

/** \brief `ImageAccessor` specialization for `BorderAccessMode::Replicated`.
 */
template <>
struct ImageAccessor<BorderAccessMode::Replicated>
{
  template <typename PixelType>
  static auto access(const Image<PixelType>& img, SignedPixelIndex x, SignedPixelIndex y);
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
inline auto ImageAccessor<BorderAccessMode::Unchecked>::access(const Image<PixelType>& img,
                                                               SignedPixelIndex x,
                                                               SignedPixelIndex y)
{
  return img(PixelIndex{static_cast<PixelIndex::value_type>(x)}, PixelIndex{static_cast<PixelIndex::value_type>(y)});
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
inline auto ImageAccessor<BorderAccessMode::ZeroPadding>::access(const Image<PixelType>& img,
                                                                 SignedPixelIndex x,
                                                                 SignedPixelIndex y)
{
  if (x < 0 || x >= static_cast<SignedPixelIndex>(img.width()) || y < 0
      || y >= static_cast<SignedPixelIndex>(img.height()))
  {
    return PixelTraits<PixelType>::zero_element;
  }

  return img(PixelIndex{static_cast<PixelIndex::value_type>(x)}, PixelIndex{static_cast<PixelIndex::value_type>(y)});
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
inline auto ImageAccessor<BorderAccessMode::Replicated>::access(const Image<PixelType>& img,
                                                                SignedPixelIndex x,
                                                                SignedPixelIndex y)
{
  if (x < 0)
  {
    x = 0;
  }
  else if (x >= static_cast<SignedPixelIndex>(img.width()))
  {
    x = static_cast<SignedPixelIndex>(img.width() - 1);
  }

  if (y < 0)
  {
    y = 0;
  }
  else if (y >= static_cast<SignedPixelIndex>(img.height()))
  {
    y = static_cast<SignedPixelIndex>(img.height() - 1);
  }

  return img(PixelIndex{static_cast<PixelIndex::value_type>(x)}, PixelIndex{static_cast<PixelIndex::value_type>(y)});
}

}  // namespace sln

#endif  // SELENE_IMG_ACCESSORS_HPP
