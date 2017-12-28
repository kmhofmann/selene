// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_INTERPOLATORS_HPP
#define SELENE_IMG_INTERPOLATORS_HPP

/// @file

#include <selene/base/Round.hpp>
#include <selene/base/Types.hpp>

#include <selene/img/Accessors.hpp>
#include <selene/img/Image.hpp>
#include <selene/img/Pixel.hpp>

#include <type_traits>

namespace sln {

/** The image interpolation mode.
 *
 * Describes the type of interpolation to use when image pixel values are accessed using fractional indices.
 */
enum class ImageInterpolationMode
{
  NearestNeighbor,  ///< Nearest neighbor interpolation.
  Bilinear  ///< Bilinear interpolation.
};

/** \brief Image interpolator structure; provides a static `interpolate` function to access image pixels according to
 * the specified interpolation mode.
 *
 * @tparam ImageInterpolationMode The interpolation mode to use.
 * @tparam BorderAccessMode The border access mode (`BorderAccessMode`) to use. Defaults to
 * `BorderAccessMode::Unchecked`.
 */
template <ImageInterpolationMode, BorderAccessMode = BorderAccessMode::Unchecked>
struct ImageInterpolator;

/** \brief Partial `ImageInterpolator` specialization for `ImageInterpolationMode::NearestNeighbor`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 */
template <BorderAccessMode AccessMode>
struct ImageInterpolator<ImageInterpolationMode::NearestNeighbor, AccessMode>
{
  template <typename PixelType, typename ScalarAccess = default_float_t>
  static auto interpolate(const Image<PixelType>& img, ScalarAccess x, ScalarAccess y) noexcept;
};

/** \brief Partial `ImageInterpolator` specialization for `ImageInterpolationMode::Bilinear`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 */
template <BorderAccessMode AccessMode>
struct ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>
{
  template <typename PixelType, typename ScalarAccess = default_float_t, typename ScalarOutputElement = default_float_t>
  static auto interpolate(const Image<PixelType>& img, ScalarAccess x, ScalarAccess y) noexcept;

  template <typename T,
            std::uint32_t nr_channels,
            typename ScalarAccess = default_float_t,
            typename ScalarOutputElement = default_float_t>
  static auto interpolate(const Image<Pixel<T, nr_channels>>& img, ScalarAccess x, ScalarAccess y) noexcept;
};


// ----------
// Implementation:

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::NearestNeighbor` and the specified `BorderAccessMode`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam PixelType The pixel type.
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`.
 */
template <BorderAccessMode AccessMode>
template <typename PixelType, typename ScalarAccess>
inline auto ImageInterpolator<ImageInterpolationMode::NearestNeighbor, AccessMode>::interpolate(
    const Image<PixelType>& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  return ImageAccessor<AccessMode>::access(img, round<SignedPixelIndex>(x), round<SignedPixelIndex>(y));
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::Bilinear` and the specified `BorderAccessMode`.
 *
 * The type `PixelType` needs to support addition, subtraction, and multiplication by a scalar value.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam PixelType The pixel type.
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @tparam ScalarOutputElement The floating point type for each pixel element; i.e. the type for each channel element.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`. Will be of type `PixelType`.
 */
template <BorderAccessMode AccessMode>
template <typename PixelType, typename ScalarAccess, typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(const Image<PixelType>& img,
                                                                                         ScalarAccess x,
                                                                                         ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  static_assert(std::is_floating_point<ScalarOutputElement>::value,
                "Output pixel channel values must be floating point.");

  const auto xf = static_cast<SignedPixelIndex>(x);
  const auto yf = static_cast<SignedPixelIndex>(y);

  const auto dx = ScalarOutputElement{x - xf};
  const auto dy = ScalarOutputElement{y - yf};

  const auto& a = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 0);
  const auto& b = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 0);
  const auto& c = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 1);
  const auto& d = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 1);

  const auto result = a + ((b - a) * dx) + ((c - a) * dy) + ((a - b - c + d) * dx * dy);
  return result;
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::Bilinear` and the specified `BorderAccessMode`.
 *
 * This is an overload for images `Image<PixelType>`, where `PixelType` is `Pixel<T, nr_channels>`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam T The pixel element type; i.e. the type of each channel element.
 * @tparam nr_channels The number of channels for a pixel.
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @tparam ScalarOutputElement The floating point type for each pixel element; i.e. the type for each channel element.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`. Will be of type
 * `Pixel<ScalarOutputElement, nr_channels>`.
 */
template <BorderAccessMode AccessMode>
template <typename T, std::uint32_t nr_channels, typename ScalarAccess, typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(
    const Image<Pixel<T, nr_channels>>& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  static_assert(std::is_floating_point<ScalarOutputElement>::value,
                "Output pixel channel values must be floating point.");

  const auto xf = static_cast<SignedPixelIndex>(x);
  const auto yf = static_cast<SignedPixelIndex>(y);

  const auto dx = ScalarOutputElement{x - xf};
  const auto dy = ScalarOutputElement{y - yf};

  const auto& a = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 0);
  const auto& b = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 0);
  const auto& c = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 1);
  const auto& d = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 1);

  Pixel<ScalarOutputElement, nr_channels> dst;
  for (std::uint32_t i = 0; i < nr_channels; ++i)  // nr_channels is known at compile-time
  {
    dst[i] = ScalarOutputElement(a[i]) + ((b[i] - a[i]) * dx) + ((c[i] - a[i]) * dy)
             + ((a[i] - b[i] - c[i] + d[i]) * dx * dy);
  }
  return dst;
}

}  // namespace sln

#endif  // SELENE_IMG_INTERPOLATORS_HPP
