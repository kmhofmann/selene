// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_CONVERSIONS_HPP
#define SELENE_IMG_IMAGE_CONVERSIONS_HPP

/// @file

#include <selene/img/Algorithms.hpp>
#include <selene/img/Image.hpp>
#include <selene/img/PixelConversions.hpp>
#include <selene/img/PixelTraits.hpp>

namespace sln {

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename PixelDst,
          typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
void convert_image(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst);

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image(const Image<PixelSrc>& img_src);

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename PixelDst,
          typename ElementType,
          typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
void convert_image(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst, ElementType alpha_value);

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename ElementType,
          typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image(const Image<PixelSrc>& img_src, ElementType alpha_value);

// ----------
// Implementation:

namespace detail {

template <PixelFormat pixel_format_dst, typename PixelSrc>
struct TargetPixelType
{
  using type = Pixel<typename PixelTraits<PixelSrc>::Element, get_nr_channels(pixel_format_dst)>;
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename = void>
struct ImageConversion;

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct ImageConversion<pixel_format_src,
                       pixel_format_dst,
                       std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
{
  template <typename PixelSrc, typename PixelDst>
  static void apply(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst)
  {
    auto transform_func = [](const PixelSrc& px) {
      return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
    };

    transform_pixels(img_src, img_dst, transform_func);
  }

  template <typename PixelSrc>
  static auto apply(const Image<PixelSrc>& img_src)
  {
    using PixelDst = typename detail::TargetPixelType<pixel_format_dst, PixelSrc>::type;

    auto transform_func = [](const PixelSrc& px) {
      return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
    };

    Image<PixelDst> img_dst;
    transform_pixels(img_src, img_dst, transform_func);
    return img_dst;
  }
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct ImageConversion<pixel_format_src,
                       pixel_format_dst,
                       std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
{
  template <typename PixelSrc, typename PixelDst, typename ElementType>
  static void apply(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst, ElementType alpha_value)
  {
    auto transform_func = [alpha_value](const PixelSrc& px) -> PixelDst {
      return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
    };

    transform_pixels(img_src, img_dst, transform_func);
  }

  template <typename PixelSrc, typename ElementType>
  static auto apply(const Image<PixelSrc>& img_src, ElementType alpha_value)
  {
    using PixelDst = typename detail::TargetPixelType<pixel_format_dst, PixelSrc>::type;

    auto transform_func = [alpha_value](const PixelSrc& px) -> PixelDst {
      return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
    };

    Image<PixelDst> img_dst;
    transform_pixels(img_src, img_dst, transform_func);
    return img_dst;
  }
};

}  // namespace detail

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::Y>(img_rgb, img_y)` will perform an RGB -> grayscale
 * conversion, writing the output to `img_y`.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The source pixel type (usually automatically deduced).
 * @tparam PixelDst The source pixel type (usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename PixelDst, typename>
inline void convert_image(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst)
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target pixel format.");
  detail::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
 *
 * Not all conversions may be supported. If the desired conversion is unsupported, this will result in an error at
 * compile-time.
 *
 * Currently, conversions from/to the following pixel formats are supported: Y, YA, RGB, BGR, RGBA, BGRA,
 * ARGB, ABGR.
 *
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::Y>(img_rgb)` will perform an RGB -> grayscale conversion,
 * returning the output image.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The source pixel type (usually automatically deduced).
 * @param img_src The source image.
 * @return The target image.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename>
inline auto convert_image(const Image<PixelSrc>& img_src)
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  return detail::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, img_y, 255)` will perform an RGB ->
 * grayscale+luminance conversion, writing the output to `img_y`.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The source pixel type (usually automatically deduced).
 * @tparam PixelDst The source pixel type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 */
template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename PixelSrc,
          typename PixelDst,
          typename ElementType,
          typename>
inline void convert_image(const Image<PixelSrc>& img_src, Image<PixelDst>& img_dst, ElementType alpha_value)
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target pixel format.");
  detail::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst, alpha_value);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, 255)` will perform an RGB -> grayscale+luminance
 * conversion, returning the output image.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam PixelSrc The source pixel type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 * @return The target image.
 */
template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename PixelSrc, typename ElementType, typename>
inline auto convert_image(const Image<PixelSrc>& img_src, ElementType alpha_value)
{
  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source pixel format.");
  return detail::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, alpha_value);
}

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_CONVERSIONS_HPP
