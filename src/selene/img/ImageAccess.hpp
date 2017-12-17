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

template <ImageInterpolationMode InterpolationMode, BorderAccessMode AccessMode, typename ImageType, typename Index,
          typename = typename std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  return ImageInterpolator<InterpolationMode, AccessMode>::interpolate(img, x, y);
}

template <ImageInterpolationMode InterpolationMode, typename ImageType, typename Index,
          typename = typename std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  return get<InterpolationMode, BorderAccessMode::Unchecked>(img, x, y);
}

template <BorderAccessMode AccessMode, typename ImageType, typename Index,
          typename = typename std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  return get<ImageInterpolationMode::Bilinear, AccessMode>(img, x, y);
}

template <typename ImageType, typename Index,
          typename = typename std::enable_if_t<std::is_floating_point<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  return get<ImageInterpolationMode::Bilinear, BorderAccessMode::Unchecked>(img, x, y);
}


template <BorderAccessMode AccessMode, typename Index, typename ImageType,
          typename = typename std::enable_if_t<std::is_integral<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  const auto si_x = static_cast<SignedPixelIndex>(x);
  const auto si_y = static_cast<SignedPixelIndex>(y);
  return ImageAccessor<AccessMode>::access(img, si_x, si_y);
}

template <typename Index, typename ImageType, typename = typename std::enable_if_t<std::is_integral<Index>::value>>
inline auto get(const ImageType& img, Index x, Index y)
{
  return get<BorderAccessMode::Unchecked>(img, x, y);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGEACCESS_HPP
