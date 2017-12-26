// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_PIXEL_CONVERSIONS_HPP
#define SELENE_IMG_PIXEL_CONVERSIONS_HPP

/// @file

#include <selene/base/Utils.hpp>

#include <selene/img/Pixel.hpp>
#include <selene/img/PixelFormat.hpp>
#include <selene/img/PixelTraits.hpp>

#include <array>
#include <limits>
#include <type_traits>

namespace sln {

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px) noexcept;

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename ElementType,
          typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept;

template <std::uint32_t N, typename T>
constexpr Pixel<T, N> y_to_n_channel(const Pixel<T, 1>& src) noexcept;


// ----------
// Implementation:

namespace detail {

struct RGBToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {0.299, 0.587, 0.114};
};

struct BGRToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {0.114, 0.587, 0.299};
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct PixelConversion;

// ------
// From Y
// ------

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::Y>
{
  template <typename T>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 1>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::YA>
{
  template <typename T>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 1>& src, const T a) noexcept
  {
    return Pixel<T, 2>(src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 1>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 1>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 1>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 1>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 1>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 1>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[0], src[0]);
  }
};

// -------
// From YA
// -------

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::Y>
{
  template <typename T>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 2>& src) noexcept
  {
    return Pixel<T, 1>(src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::YA>
{
  template <typename T>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 2>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 2>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> ya_to_bgr(const Pixel<T, 2>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 2>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 2>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 2>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 2>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[0], src[0]);
  }
};

// --------
// From RGB
// --------

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src), a};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::RGBToYCoefficients>(src), a};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 3>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 3>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[1], src[2], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[2], src[1], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[2], src[1], src[0]);
  }
};

// --------
// From BGR
// --------

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src), a};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::BGRToYCoefficients>(src), a};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 3>& src) noexcept
  {
    return Pixel<T, 3>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 3>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[2], src[1], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(src[0], src[1], src[2], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 3>& src, const T a) noexcept
  {
    return Pixel<T, 4>(a, src[0], src[1], src[2]);
  }
};

//// ---------
//// From RGBA
//// ---------

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::RGBToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src), src[3]};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::RGBToYCoefficients>(src), src[3]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[2], src[1], src[0], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
  }
};

// ---------
// From BGRA
// ---------

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::BGRToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src), src[3]};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::BGRToYCoefficients>(src), src[3]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[2], src[1], src[0], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[0], src[1], src[2]);
  }
};

// ---------
// From ARGB
// ---------

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1), src[0]};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::RGBToYCoefficients>(src.data() + 1), src[0]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[1], src[2], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[1], src[2], src[3], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[0], src[3], src[2], src[1]);
  }
};

// ---------
// From ABGR
// ---------

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::Y>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1)};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 1>{linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::YA>
{
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{approximate_linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1), src[0]};
  }

  template <typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 2>{linear_combination<T, 3, detail::BGRToYCoefficients>(src.data() + 1), src[0]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::RGB>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::BGR>
{
  template <typename T>
  static constexpr Pixel<T, 3> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 3>(src[1], src[2], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::RGBA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::BGRA>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[1], src[2], src[3], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::ARGB>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return Pixel<T, 4>(src[0], src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::ABGR>
{
  template <typename T>
  static constexpr Pixel<T, 4> apply(const Pixel<T, 4>& src) noexcept
  {
    return src;
  }
};

}  // namespace detail

/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::RGB, PixelFormat::Y>(Pixel_8u3(255, 0, 0))` will return a `Pixel_8u1` instance,
 * performing an RGB -> grayscale conversion.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The pixel type (usually automatically deduced).
 * @param px The pixel value to convert.
 * @return A pixel value in the target format.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename>
inline constexpr auto convert_pixel(const PixelSrc& px) noexcept
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  return detail::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
}

/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * This is an overload for performing conversions that add an alpha channel (e.g. RGB -> RGBA).
 * In this case, the additional alpha value has to be manually specified.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::RGB, PixelFormat::Y>(Pixel_8u3(255, 0, 0))` will return a `Pixel_8u1` instance,
 * performing an RGB -> grayscale conversion.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The pixel type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param px The pixel value to convert.
 * @param alpha_value The alpha value to assign to the output pixel.
 * @return A pixel value in the target format.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename ElementType, typename>
inline constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  return detail::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
}

/** \brief Converts a one-channel pixel value to an n-channel pixel value, replicating the source value for each
 * channel.
 *
 * @tparam N The number of output channels.
 * @tparam T The pixel type (input/output).
 * @param src The source pixel.
 * @return The resulting n-channel pixel.
 */
template <std::uint32_t N, typename T>
constexpr Pixel<T, N> y_to_n_channel(const Pixel<T, 1>& src) noexcept
{
  return Pixel<T, N>(make_array_n_equal<T, N>(src[0]));
}

}  // namespace sln

#endif  // SELENE_IMG_PIXEL_CONVERSIONS_HPP
