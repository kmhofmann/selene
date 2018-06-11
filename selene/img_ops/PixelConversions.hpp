// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
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

// Overloads for unknown source pixel format

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename = std::enable_if_t<PixelTraits<PixelSrc>::pixel_format == PixelFormat::Unknown>,
          typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px) noexcept;

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename ElementType,
          typename = std::enable_if_t<PixelTraits<PixelSrc>::pixel_format == PixelFormat::Unknown>,
          typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept;

// Overloads for known source pixel format

template <PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename = std::enable_if_t<PixelTraits<PixelSrc>::pixel_format != PixelFormat::Unknown>,
          typename = std::enable_if_t<!conversion_requires_alpha_value(PixelTraits<PixelSrc>::pixel_format, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px) noexcept;

template <PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename ElementType,
          typename = std::enable_if_t<PixelTraits<PixelSrc>::pixel_format != PixelFormat::Unknown>,
          typename = std::enable_if_t<conversion_requires_alpha_value(PixelTraits<PixelSrc>::pixel_format, pixel_format_dst)>>
constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept;

// Channel expansion convenience function

template <std::size_t N, typename T>
constexpr Pixel<T, N> y_to_n_channel(const Pixel<T, 1>& src) noexcept;


// ----------
// Implementation:

namespace impl {

struct RGBToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {{0.299, 0.587, 0.114}};
};

struct BGRToYCoefficients
{
  static constexpr std::array<default_float_t, 3> values = {{0.114, 0.587, 0.299}};
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct PixelConversion;

// ------
// From Y
// ------

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 1, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 1, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>(src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 1, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 1, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 1, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 1, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 1, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(a, src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::Y, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 1, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(a, src[0], src[0], src[0]);
  }
};

// -------
// From YA
// -------

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 2, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>(src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 2, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 2, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, sln::PixelFormat::BGR> ya_to_bgr(const Pixel<T, 2, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, sln::PixelFormat::BGR>(src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, sln::PixelFormat::RGBA> apply(const Pixel<T, 2, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, sln::PixelFormat::RGBA>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, sln::PixelFormat::BGRA> apply(const Pixel<T, 2, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, sln::PixelFormat::BGRA>(src[0], src[0], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, sln::PixelFormat::ARGB> apply(const Pixel<T, 2, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, sln::PixelFormat::ARGB>(a, src[0], src[0], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::YA, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, sln::PixelFormat::ABGR> apply(const Pixel<T, 2, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, sln::PixelFormat::ABGR>(a, src[0], src[0], src[0]);
  }
};

// --------
// From RGB
// --------

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::RGBToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src), a};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::RGBToYCoefficients>(src), a};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[0], src[1], src[2], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[2], src[1], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(a, src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGB, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(a, src[2], src[1], src[0]);
  }
};

// --------
// From BGR
// --------

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::BGRToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src), a};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::BGRToYCoefficients>(src), a};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 3, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
    static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[2], src[1], src[0], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[0], src[1], src[2], a);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(a, src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGR, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 3, pixel_format>& src, const T a) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(a, src[0], src[1], src[2]);
  }
};

//// ---------
//// From RGBA
//// ---------

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::RGBToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src), src[3]};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::RGBToYCoefficients>(src), src[3]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[2], src[1], src[0], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(src[3], src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::RGBA, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(src[3], src[2], src[1], src[0]);
  }
};

// ---------
// From BGRA
// ---------

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::BGRToYCoefficients>(src)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src), src[3]};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::BGRToYCoefficients>(src), src[3]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[0], src[1], src[2]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[2], src[1], src[0], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::BGRA, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(src[3], src[0], src[1], src[2]);
  }
};

// ---------
// From ARGB
// ---------

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src.data() + 1)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::RGBToYCoefficients>(src.data() + 1)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::RGBToYCoefficients>(src.data() + 1), src[0]};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::RGBToYCoefficients>(src.data() + 1), src[0]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[1], src[2], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[1], src[2], src[3], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return src;
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ARGB, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ABGR>(src[0], src[3], src[2], src[1]);
  }
};

// ---------
// From ABGR
// ---------

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::Y>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src.data() + 1)};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 1, PixelFormat::Y> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 1, PixelFormat::Y>{linear_combination<T, 3, impl::BGRToYCoefficients>(src.data() + 1)};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::YA>
{
  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_integral<T>::value>>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{approximate_linear_combination<T, 3, impl::BGRToYCoefficients>(src.data() + 1), src[0]};
  }

  template <typename T, PixelFormat pixel_format, typename = std::enable_if_t<std::is_floating_point<T>::value>, typename = void>
  static constexpr Pixel<T, 2, PixelFormat::YA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 2, PixelFormat::YA>{linear_combination<T, 3, impl::BGRToYCoefficients>(src.data() + 1), src[0]};
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::RGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::RGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::RGB>(src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::BGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 3, PixelFormat::BGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 3, PixelFormat::BGR>(src[1], src[2], src[3]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::RGBA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::RGBA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::RGBA>(src[3], src[2], src[1], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::BGRA>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::BGRA> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::BGRA>(src[1], src[2], src[3], src[0]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::ARGB>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ARGB> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return Pixel<T, 4, PixelFormat::ARGB>(src[0], src[3], src[2], src[1]);
  }
};

template <>
struct PixelConversion<sln::PixelFormat::ABGR, sln::PixelFormat::ABGR>
{
  template <typename T, PixelFormat pixel_format>
  static constexpr Pixel<T, 4, PixelFormat::ABGR> apply(const Pixel<T, 4, pixel_format>& src) noexcept
  {
    return src;
  }
};

}  // namespace impl


// Overloads for unknown source pixel format


/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * This overload is valid if the source pixel has pixel format PixelFormat::Unknown. In this case, the source format
 * will have to be explicitly specified as first template parameter.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::RGB, PixelFormat::Y>(Pixel_8u3(255, 0, 0))` will return a `PixelY_8u` instance,
 * performing an RGB -> grayscale conversion.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The pixel type (usually automatically deduced).
 * @param px The pixel value to convert.
 * @return A pixel value in the target format.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename, typename>
inline constexpr auto convert_pixel(const PixelSrc& px) noexcept
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  static_assert(pixel_format_src == PixelTraits<PixelSrc>::pixel_format
                || PixelTraits<PixelSrc>::pixel_format == PixelFormat::Unknown,
                "Pixel format mismatch");
  return impl::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
}

/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * This is an overload for performing conversions that add an alpha channel (e.g. RGB -> RGBA).
 * In this case, the additional alpha value has to be manually specified.
 *
 * This overload is valid if the source pixel has pixel format PixelFormat::Unknown. In this case, the source format
 * will have to be explicitly specified as first template parameter.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::RGB, PixelFormat::Y>(Pixel_8u3(255, 0, 0))` will return a `PixelY_8u` instance,
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
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename ElementType, typename, typename>
inline constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  static_assert(pixel_format_src == PixelTraits<PixelSrc>::pixel_format
                || PixelTraits<PixelSrc>::pixel_format == PixelFormat::Unknown,
                "Pixel format mismatch");
  return impl::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
}


// Overloads for known source pixel format


/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * This overload is valid if the source pixel has a known pixel format, i.e. pixel format is not PixelFormat::Unknown.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::Y>(PixelRGB_8u(255, 0, 0))` will return a `PixelY_8u` instance,
 * performing an RGB -> grayscale conversion.
 *
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The pixel type (usually automatically deduced).
 * @param px The pixel value to convert.
 * @return A pixel value in the target format.
 */
template <PixelFormat pixel_format_dst, typename PixelSrc, typename, typename>
inline constexpr auto convert_pixel(const PixelSrc& px) noexcept
{
  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
}

/** \brief Converts a pixel value from a source to a target pixel format.
 *
 * This is an overload for performing conversions that add an alpha channel (e.g. RGB -> RGBA).
 * In this case, the additional alpha value has to be manually specified.
 *
 * This overload is valid if the source pixel has a known pixel format, i.e. pixel format is not PixelFormat::Unknown.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_pixel<PixelFormat::Y>(PixelRGB_8u(255, 0, 0))` will return a `PixelY_8u` instance,
 * performing an RGB -> grayscale conversion.
 *
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The pixel type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param px The pixel value to convert.
 * @param alpha_value The alpha value to assign to the output pixel.
 * @return A pixel value in the target format.
 */
template <PixelFormat pixel_format_dst, typename PixelSrc, typename ElementType, typename, typename>
inline constexpr auto convert_pixel(const PixelSrc& px, ElementType alpha_value) noexcept
{
  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
}

/** \brief Converts a one-channel pixel value to an n-channel pixel value, replicating the source value for each
 * channel.
 *
 * @tparam N The number of output channels.
 * @tparam T The pixel type (input/output).
 * @param src The source pixel.
 * @return The resulting n-channel pixel.
 */
template <std::size_t N, typename T>
constexpr Pixel<T, N> y_to_n_channel(const Pixel<T, 1>& src) noexcept
{
  return Pixel<T, N>(make_array_n_equal<T, N>(src[0]));
}

}  // namespace sln

#endif  // SELENE_IMG_PIXEL_CONVERSIONS_HPP
