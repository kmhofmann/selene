// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGEACCESS_HPP
#define SELENE_IMG_IMAGEACCESS_HPP

/// @file

#include <selene/base/Types.hpp>

#include <selene/img/Accessors.hpp>
#include <selene/img/Image.hpp>
#include <selene/img/Interpolators.hpp>

#include <type_traits>

namespace sln {

/** \brief Returns the pixel value of the image at the specified (floating point) location, using the specified
 * interpolation method and border access mode.
 *
 * @tparam InterpolationMode The interpolation mode to use. Defaults to `ImageInterpolationMode::Bilinear`.
 * @tparam AccessMode The border access mode to use. Defaults to BorderAccessMode::Unchecked.
 * @tparam ImageType The image type, i.e. `Image<...>`.
 * @tparam Index The index type. Must be floating point for this overload.
 * @param img The image to return the pixel value from.
 * @param x The floating point x-coordinate.
 * @param y The floating point y-coordinate.
 * @return Interpolated pixel value at the specified (x, y) location.
 */
template <ImageInterpolationMode InterpolationMode = ImageInterpolationMode::Bilinear,
          BorderAccessMode AccessMode = BorderAccessMode::Unchecked,
          typename ImageType,
          typename Index,
          typename = std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y) noexcept
{
  return ImageInterpolator<InterpolationMode, AccessMode>::interpolate(img, x, y);
}

/** \brief Returns the pixel value of the image at the specified (floating point) location, using bilinear interpolation
 * and the specified border access mode.
 *
 * This is a function overload where the border access mode can be selected, but the interpolation method is the default
 * (bilinear interpolation).
 *
 * @tparam AccessMode The border access mode to use.
 * @tparam ImageType The image type, i.e. `Image<...>`.
 * @tparam Index The index type. Must be floating point for this overload.
 * @param img The image to return the pixel value from.
 * @param x The floating point x-coordinate.
 * @param y The floating point y-coordinate.
 * @return Interpolated pixel value at the specified (x, y) location.
 */
template <BorderAccessMode AccessMode,
          typename ImageType,
          typename Index,
          typename = std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y) noexcept
{
  return get<ImageInterpolationMode::Bilinear, AccessMode>(img, x, y);
}

/** \brief Returns the pixel value of the image at the specified (integral) location, using the specified border access
 * mode.
 *
 * @tparam AccessMode The border access mode to use. Defaults to BorderAccessMode::Unchecked.
 * @tparam ImageType The image type, i.e. `Image<...>`.
 * @tparam Index The index type. Must be floating point for this overload.
 * @param img The image to return the pixel value from.
 * @param x The floating point x-coordinate.
 * @param y The floating point y-coordinate.
 * @return Pixel value at the specified (x, y) location.
 */
template <BorderAccessMode AccessMode = BorderAccessMode::Unchecked,
          typename ImageType,
          typename Index,
          typename = std::enable_if_t<std::is_integral<Index>::value>,
          typename = void>
inline auto get(const ImageType& img, Index x, Index y) noexcept
{
  const auto si_x = static_cast<SignedPixelIndex>(x);
  const auto si_y = static_cast<SignedPixelIndex>(y);
  return ImageAccessor<AccessMode>::access(img, si_x, si_y);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGEACCESS_HPP
