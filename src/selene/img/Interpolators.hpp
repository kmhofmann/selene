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

template <ImageInterpolationMode, BorderAccessMode = BorderAccessMode::Unchecked>
struct ImageInterpolator;

template <BorderAccessMode AccessMode>
struct ImageInterpolator<ImageInterpolationMode::NearestNeighbor, AccessMode>
{
  template <typename PixelType, typename ScalarAccess = default_float_t>
  static auto interpolate(const Image<PixelType>& img, ScalarAccess x, ScalarAccess y)
  {
    x = round<SignedPixelIndex>(x);
    y = round<SignedPixelIndex>(y);
    return ImageAccessor<AccessMode>::access(img, x, y);
  }
};

template <BorderAccessMode AccessMode>
struct ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>
{
  template <typename PixelType, typename ScalarAccess = default_float_t, typename ScalarOutputElement = default_float_t>
  static auto interpolate(const Image<PixelType>& img, ScalarAccess x, ScalarAccess y)
  {
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

  template <typename T, std::uint32_t nr_channels, typename ScalarAccess = default_float_t,
            typename ScalarOutputElement = default_float_t>
  static auto interpolate(const Image<Pixel<T, nr_channels>>& img, ScalarAccess x, ScalarAccess y)
  {
    const auto xf = static_cast<SignedPixelIndex>(x);
    const auto yf = static_cast<SignedPixelIndex>(y);

    const auto dx = ScalarOutputElement{x - xf};
    const auto dy = ScalarOutputElement{y - yf};

    const auto& a = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 0);
    const auto& b = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 0);
    const auto& c = ImageAccessor<AccessMode>::access(img, xf + 0, yf + 1);
    const auto& d = ImageAccessor<AccessMode>::access(img, xf + 1, yf + 1);

    Pixel<ScalarOutputElement, nr_channels> dst;
    for (std::uint32_t i = 0; i < nr_channels; ++i)
    {
      dst[i] = ScalarOutputElement(a[i]) + ((b[i] - a[i]) * dx) + ((c[i] - a[i]) * dy)
               + ((a[i] - b[i] - c[i] + d[i]) * dx * dy);
    }
    return dst;
  }
};

}  // namespace sln

#endif  // SELENE_IMG_INTERPOLATORS_HPP
