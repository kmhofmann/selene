// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPS_IMAGE_CONVERSIONS_HPP
#define SELENE_IMG_OPS_IMAGE_CONVERSIONS_HPP

/// @file

#include <selene/img/pixel/PixelTraits.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/Algorithms.hpp>
#include <selene/img_ops/Clone.hpp>
#include <selene/img_ops/PixelConversions.hpp>
#include <selene/img_ops/_impl/ImageConversionExpr.hpp>

namespace sln {

/// \addtogroup group-img-ops
/// @{

// Overloads for known source pixel format

template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
          typename = std::enable_if_t<!conversion_requires_alpha_value(PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst);

template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename ElementType,
          typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
          typename = std::enable_if_t<conversion_requires_alpha_value(PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, ElementType alpha_value);

template <PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
    typename = std::enable_if_t<!conversion_requires_alpha_value(PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
auto convert_image(const ImageBase<DerivedSrc>& img_src);

template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename ElementType,
          typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
          typename = std::enable_if_t<conversion_requires_alpha_value(PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
auto convert_image(const ImageBase<DerivedSrc>& img_src, ElementType alpha_value);

template <PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename = std::enable_if_t<PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
    typename = std::enable_if_t<!conversion_requires_alpha_value(PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src);

template <PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename ElementType,
    typename = std::enable_if_t<PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format != PixelFormat::Unknown>,
    typename = std::enable_if_t<conversion_requires_alpha_value(PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format, pixel_format_dst)>>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src, ElementType alpha_value);

// Overloads for unknown source pixel format

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename DerivedDst,
    typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst);

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename DerivedDst,
    typename ElementType,
    typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, ElementType alpha_value);

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image(const ImageBase<DerivedSrc>& img_src);

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename ElementType,
    typename = std::enable_if_t<PixelTraits<typename ImageBase<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image(const ImageBase<DerivedSrc>& img_src, ElementType alpha_value);

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename = std::enable_if_t<PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src);

template <PixelFormat pixel_format_src,
    PixelFormat pixel_format_dst,
    typename DerivedSrc,
    typename ElementType,
    typename = std::enable_if_t<PixelTraits<typename ImageExpr<DerivedSrc>::PixelType>::pixel_format == PixelFormat::Unknown>,
    typename = std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src, ElementType alpha_value);

/// @}

// ----------
// Implementation:

namespace impl {

template <PixelFormat pixel_format_dst, typename PixelSrc>
struct TargetPixelType
{
  using TargetElement = typename PixelTraits<PixelSrc>::Element;
  constexpr static auto target_nr_channels = get_nr_channels(pixel_format_dst);
  using type = Pixel<TargetElement, target_nr_channels, pixel_format_dst>;
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst, typename = void>
struct ImageConversion;

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct ImageConversion<pixel_format_src,
                       pixel_format_dst,
                       std::enable_if_t<!conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
{
  template <typename DerivedSrc, typename DerivedDst>
  static void apply(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
  {
    using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
    using PixelDst = typename ImageBase<DerivedDst>::PixelType;

    if constexpr (std::is_same_v<PixelSrc, PixelDst>)
    {
      clone(img_src, img_dst);
    }
    else
    {
      auto transform_func = [](const PixelSrc& px) {
        return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px);
      };

      transform_pixels(img_src, img_dst, transform_func);
    }
  }

  template <typename DerivedSrc>
  static auto apply(const ImageBase<DerivedSrc>& img_src)
  {
    using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
    using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

    Image<PixelDst> img_dst;
    ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst);
    return img_dst;
  }
};

template <PixelFormat pixel_format_src, PixelFormat pixel_format_dst>
struct ImageConversion<pixel_format_src,
                       pixel_format_dst,
                       std::enable_if_t<conversion_requires_alpha_value(pixel_format_src, pixel_format_dst)>>
{
  template <typename DerivedSrc, typename DerivedDst, typename ElementType>
  static void apply(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst,
                    [[maybe_unused]] ElementType alpha_value)
  {
    using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
    using PixelDst = typename ImageBase<DerivedDst>::PixelType;

    if constexpr (std::is_same_v<PixelSrc, PixelDst>)
    {
      clone(img_src, img_dst);
    }
    else
    {
      auto transform_func = [alpha_value](const PixelSrc& px) -> PixelDst {
        return PixelConversion<pixel_format_src, pixel_format_dst>::apply(px, alpha_value);
      };

      transform_pixels(img_src, img_dst, transform_func);
    }
  }

  template <typename DerivedSrc, typename ElementType>
  static auto apply(const ImageBase<DerivedSrc>& img_src, ElementType alpha_value)
  {
    using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
    using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

    Image<PixelDst> img_dst;
    ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst, alpha_value);
    return img_dst;
  }
};

}  // namespace impl


// Overloads for known source pixel format


/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload is valid if the source pixel has a known pixel format, i.e. pixel format is not PixelFormat::Unknown.
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
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 */
template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename,
          typename>
inline void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelDst = typename ImageBase<DerivedDst>::PixelType;

  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target number of channels for given pixel format.");
  static_assert(PixelTraits<PixelDst>::pixel_format == pixel_format_dst
                || PixelTraits<PixelDst>::pixel_format == PixelFormat::Unknown,
                "Incorrect target pixel format.");

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, img_y, 255)` will perform an RGB ->
 * grayscale+luminance conversion, writing the output to `img_y`.
 *
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 */
template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename ElementType,
          typename,
          typename>
inline void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, ElementType alpha_value)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelDst = typename ImageBase<DerivedDst>::PixelType;

  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target number of channels for given pixel format.");
  static_assert(PixelTraits<PixelDst>::pixel_format == pixel_format_dst
                || PixelTraits<PixelDst>::pixel_format == PixelFormat::Unknown,
                "Incorrect target pixel format.");

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst, alpha_value);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
 *
 * This overload is valid if the source pixel has a known pixel format, i.e. pixel format is not PixelFormat::Unknown.
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
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @param img_src The source image.
 * @return The target image.
 */
template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename,
          typename>
inline auto convert_image(const ImageBase<DerivedSrc>& img_src)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, 255)` will perform an RGB -> grayscale+luminance
 * conversion, returning the output image.
 *
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 * @return The target image.
 */
template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename ElementType,
          typename,
          typename>
inline auto convert_image(const ImageBase<DerivedSrc>& img_src, ElementType alpha_value)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, alpha_value);
}

template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename,
          typename>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src)
{
  using PixelSrc = typename ImageExpr<DerivedSrc>::PixelType;
  using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::ImageConversionExpr<pixel_format_src, pixel_format_dst, PixelSrc, PixelDst, ImageExpr<DerivedSrc>>(img_src);
}

template <PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename ElementType,
          typename,
          typename>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src, ElementType alpha_value)
{
  using PixelSrc = typename ImageExpr<DerivedSrc>::PixelType;
  using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

  constexpr auto pixel_format_src = PixelTraits<PixelSrc>::pixel_format;
  return impl::ImageConversionAlphaExpr<pixel_format_src, pixel_format_dst, PixelSrc, PixelDst, ElementType, ImageExpr<DerivedSrc>>(img_src, alpha_value);
}


// Overloads for unknown source pixel format


/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::Y>(img_rgb, img_y)` will perform an RGB -> grayscale
 * conversion, writing the output to `img_y`.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 */
template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename,
          typename>
inline void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelDst = typename ImageBase<DerivedDst>::PixelType;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");
  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target number of channels for given pixel format.");

  impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, img_y, 255)` will perform an RGB ->
 * grayscale+luminance conversion, writing the output to `img_y`.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam DerivedDst The typed target image type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param img_dst The target image. Its pixel type has to be compatible with the target pixel format.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 */
template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename DerivedDst,
          typename ElementType,
          typename,
          typename>
inline void convert_image(const ImageBase<DerivedSrc>& img_src, ImageBase<DerivedDst>& img_dst, ElementType alpha_value)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;
  using PixelDst = typename ImageBase<DerivedDst>::PixelType;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");
  static_assert(get_nr_channels(pixel_format_dst) == PixelTraits<PixelDst>::nr_channels,
                "Incorrect target number of channels for given pixel format.");

  impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, img_dst, alpha_value);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::Y>(img_rgb)` will perform an RGB -> grayscale conversion,
 * returning the output image.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @param img_src The source image.
 * @return The target image.
 */
template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename,
          typename>
inline auto convert_image(const ImageBase<DerivedSrc>& img_src)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");

  return impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src);
}

/** \brief Converts an image (i.e. each pixel) from a source to a target pixel format.
 *
 * This overload returns the target image, for which the type is automatically determined.
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
 * Example: `convert_image<PixelFormat::RGB, PixelFormat::YA>(img_rgb, 255)` will perform an RGB -> grayscale+luminance
 * conversion, returning the output image.
 *
 * @tparam pixel_format_src The source pixel format.
 * @tparam pixel_format_dst The target pixel format.
 * @tparam DerivedSrc The typed source image type (usually automatically deduced).
 * @tparam ElementType The pixel element type (for the target format; usually automatically deduced).
 * @param img_src The source image.
 * @param alpha_value The alpha value to assign to each pixel in the target image.
 * @return The target image.
 */
template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename ElementType,
          typename,
          typename>
inline auto convert_image(const ImageBase<DerivedSrc>& img_src, ElementType alpha_value)
{
  using PixelSrc = typename ImageBase<DerivedSrc>::PixelType;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");

  return impl::ImageConversion<pixel_format_src, pixel_format_dst>::apply(img_src, alpha_value);
}

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename,
          typename>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src)
{
  using PixelSrc = typename ImageExpr<DerivedSrc>::PixelType;
  using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");

  return impl::ImageConversionExpr<pixel_format_src, pixel_format_dst, PixelSrc, PixelDst, ImageExpr<DerivedSrc>>(img_src);
}

template <PixelFormat pixel_format_src,
          PixelFormat pixel_format_dst,
          typename DerivedSrc,
          typename ElementType,
          typename,
          typename>
auto convert_image_expr(const ImageExpr<DerivedSrc>& img_src, ElementType alpha_value)
{
  using PixelSrc = typename ImageExpr<DerivedSrc>::PixelType;
  using PixelDst = typename impl::TargetPixelType<pixel_format_dst, PixelSrc>::type;

  static_assert(get_nr_channels(pixel_format_src) == PixelTraits<PixelSrc>::nr_channels,
                "Incorrect source number of channels for given pixel format.");

  return impl::ImageConversionAlphaExpr<pixel_format_src, pixel_format_dst, PixelSrc, PixelDst, ElementType, ImageExpr<DerivedSrc>>(img_src, alpha_value);
}

}  // namespace sln

#endif  // SELENE_IMG_OPS_IMAGE_CONVERSIONS_HPP
