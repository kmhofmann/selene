// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_CONVERSIONS_HPP
#define SELENE_IMG_PIXEL_CONVERSIONS_HPP

/// @file

#include <selene/base/Utils.hpp>

#include <selene/img/Pixel.hpp>
#include <selene/img/PixelTraits.hpp>

#include <array>
#include <limits>
#include <type_traits>

namespace sln {

namespace detail {

struct RGBToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {0.299, 0.587, 0.114};
};

struct BGRToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {0.114, 0.587, 0.299};
};

}  // namespace detail

// ------
// From Y
// ------

// TODO: Documentation (for all of these)
template <std::uint32_t N, typename T>
constexpr Pixel<T, N> y_to_n_channel(const Pixel<T, 1>& src)
{
  return Pixel<T, N>(make_array_n_equal<T, N>(src[0]));
}

template <typename T>
constexpr Pixel<T, 2> y_to_ya(const Pixel<T, 1>& src, const T a)
{
  return Pixel<T, 2>(src[0], a);
}

template <typename T>
constexpr Pixel<T, 3> y_to_rgb(const Pixel<T, 1>& src)
{
  return Pixel<T, 3>(src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 3> y_to_bgr(const Pixel<T, 1>& src)
{
  return Pixel<T, 3>(src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> y_to_rgba(const Pixel<T, 1>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[0], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> y_to_bgra(const Pixel<T, 1>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[0], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> y_to_argb(const Pixel<T, 1>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> y_to_abgr(const Pixel<T, 1>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[0], src[0]);
}

// -------
// From YA
// -------

template <typename T>
constexpr Pixel<T, 1> ya_to_y(const Pixel<T, 2>& src)
{
  return Pixel<T, 1>(src[0]);
}

template <typename T>
constexpr Pixel<T, 3> ya_to_rgb(const Pixel<T, 2>& src)
{
  return Pixel<T, 3>(src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 3> ya_to_bgr(const Pixel<T, 2>& src)
{
  return Pixel<T, 3>(src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> ya_to_rgba(const Pixel<T, 2>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[0], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> ya_to_bgra(const Pixel<T, 2>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[0], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> ya_to_argb(const Pixel<T, 2>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[0], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> ya_to_abgr(const Pixel<T, 2>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[0], src[0]);
}

// --------
// From RGB
// --------

template <typename T>
constexpr Pixel<T, 1> rgb_to_y(const Pixel<T, 3>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
}

template <typename T>
constexpr Pixel<T, 2> rgb_to_ya(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src), a};
}

template <typename T>
constexpr Pixel<T, 3> rgb_to_bgr(const Pixel<T, 3>& src)
{
  return Pixel<T, 3>(src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> rgb_to_rgba(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[1], src[2], a);
}

template <typename T>
constexpr Pixel<T, 4> rgb_to_bgra(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(src[2], src[1], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> rgb_to_argb(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[1], src[2]);
}

template <typename T>
constexpr Pixel<T, 4> rgb_to_abgr(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(a, src[2], src[1], src[0]);
}

// --------
// From BGR
// --------

template <typename T>
constexpr Pixel<T, 1> bgr_to_y(const Pixel<T, 3>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
}

template <typename T>
constexpr Pixel<T, 2> bgr_to_ya(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src), a};
}

template <typename T>
constexpr Pixel<T, 3> bgr_to_rgb(const Pixel<T, 3>& src)
{
  return Pixel<T, 3>(src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> bgr_to_rgba(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(src[2], src[1], src[0], a);
}

template <typename T>
constexpr Pixel<T, 4> bgr_to_bgra(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(src[0], src[1], src[2], a);
}

template <typename T>
constexpr Pixel<T, 4> bgr_to_argb(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(a, src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> bgr_to_abgr(const Pixel<T, 3>& src, const T a)
{
  return Pixel<T, 4>(a, src[0], src[1], src[2]);
}

// ---------
// From RGBA
// ---------

template <typename T>
constexpr Pixel<T, 1> rgba_to_y(const Pixel<T, 4>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
}

template <typename T>
constexpr Pixel<T, 2> rgba_to_ya(const Pixel<T, 4>& src)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src), src[3]};
}

template <typename T>
constexpr Pixel<T, 3> rgba_to_rgb(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[0], src[1], src[2]);
}

template <typename T>
constexpr Pixel<T, 3> rgba_to_bgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> rgba_to_bgra(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[2], src[1], src[0], src[3]);
}

template <typename T>
constexpr Pixel<T, 4> rgba_to_argb(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[0], src[1], src[2]);
}

template <typename T>
constexpr Pixel<T, 4> rgba_to_abgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
}

// ---------
// From BGRA
// ---------

template <typename T>
constexpr Pixel<T, 1> bgra_to_y(const Pixel<T, 4>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
}

template <typename T>
constexpr Pixel<T, 2> bgra_to_ya(const Pixel<T, 4>& src)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src), src[3]};
}

template <typename T>
constexpr Pixel<T, 3> bgra_to_rgb(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 3> bgra_to_bgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[0], src[1], src[2]);
}

template <typename T>
constexpr Pixel<T, 4> bgra_to_rgba(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[2], src[1], src[0], src[3]);
}

template <typename T>
constexpr Pixel<T, 4> bgra_to_argb(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> bgra_to_abgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[0], src[1], src[2]);
}

// ---------
// From ARGB
// ---------

template <typename T>
constexpr Pixel<T, 1> argb_to_y(const Pixel<T, 4>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1)};
}

template <typename T>
constexpr Pixel<T, 2> argb_to_ya(const Pixel<T, 4>& src)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1), src[0]};
}

template <typename T>
constexpr Pixel<T, 3> argb_to_rgb(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[1], src[2], src[3]);
}

template <typename T>
constexpr Pixel<T, 3> argb_to_bgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[3], src[2], src[1]);
}

template <typename T>
constexpr Pixel<T, 4> argb_to_rgba(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[1], src[2], src[3], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> argb_to_bgra(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> argb_to_abgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[0], src[3], src[2], src[1]);
}

// ---------
// From ABGR
// ---------

template <typename T>
constexpr Pixel<T, 1> abgr_to_y(const Pixel<T, 4>& src)
{
  return Pixel<T, 1>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1)};
}

template <typename T>
constexpr Pixel<T, 2> abgr_to_ya(const Pixel<T, 4>& src)
{
  return Pixel<T, 2>{rounded_linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1), src[0]};
}

template <typename T>
constexpr Pixel<T, 3> abgr_to_rgb(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[3], src[2], src[1]);
}

template <typename T>
constexpr Pixel<T, 3> abgr_to_bgr(const Pixel<T, 4>& src)
{
  return Pixel<T, 3>(src[1], src[2], src[3]);
}

template <typename T>
constexpr Pixel<T, 4> abgr_to_rgba(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> abgr_to_bgra(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[1], src[2], src[3], src[0]);
}

template <typename T>
constexpr Pixel<T, 4> abgr_to_argb(const Pixel<T, 4>& src)
{
  return Pixel<T, 4>(src[0], src[3], src[2], src[1]);
}


}  // namespace sln

#endif  // SELENE_IMG_PIXEL_CONVERSIONS_HPP
