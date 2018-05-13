// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGEACCESS_HPP
#define SELENE_IMG_IMAGEACCESS_HPP

/// @file

#include <selene/base/Types.hpp>

#include <selene/img/BorderAccessors.hpp>
#include <selene/img/Image.hpp>
#include <selene/img/Interpolators.hpp>

#include <type_traits>

namespace sln {

// The following function definitions employ some tricks to coerce Visual C++ into choosing the right overloads
// (tested with Visual Studio v15.5.2). The std::integral_constant<> parameters should not be needed, and the
// std::enable_if<> statements could all be part of the template parameters.

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
inline auto get(const ImageType& img,
                Index x,
                Index y,
                std::integral_constant<ImageInterpolationMode, InterpolationMode> = {}) noexcept
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
inline auto get(const ImageType& img,
                Index x,
                Index y,
                std::integral_constant<BorderAccessMode, AccessMode> = {}) noexcept
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
template <BorderAccessMode AccessMode = BorderAccessMode::Unchecked, typename ImageType>
inline auto get(const ImageType& img,
                PixelIndex x,
                PixelIndex y) noexcept
{
  return ImageBorderAccessor<AccessMode>::access(img, x, y);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGEACCESS_HPP
