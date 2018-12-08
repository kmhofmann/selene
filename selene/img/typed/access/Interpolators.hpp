// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_INTERPOLATORS_HPP
#define SELENE_IMG_INTERPOLATORS_HPP

/// @file

#include <selene/base/Round.hpp>
#include <selene/base/Types.hpp>

#include <selene/img/pixel/Pixel.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <selene/img/typed/access/BorderAccessors.hpp>

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
  constexpr static PixelLength::value_type index_to_left = 0;
  constexpr static PixelLength::value_type index_to_right = 1;
  constexpr static PixelLength::value_type index_to_up = 0;
  constexpr static PixelLength::value_type index_to_down = 1;

  template <typename ImageType, typename ScalarAccess = default_float_t>
  static auto interpolate(const ImageType& img, ScalarAccess x, ScalarAccess y) noexcept;

  template <typename PixelType, typename ScalarAccess = default_float_t>
  static auto interpolate(const RelativeAccessor<PixelType>& img, ScalarAccess x, ScalarAccess y) noexcept;
};

/** \brief Partial `ImageInterpolator` specialization for `ImageInterpolationMode::Bilinear`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 */
template <BorderAccessMode AccessMode>
struct ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>
{
  constexpr static PixelLength::value_type index_to_left = 0;
  constexpr static PixelLength::value_type index_to_right = 1;
  constexpr static PixelLength::value_type index_to_up = 0;
  constexpr static PixelLength::value_type index_to_down = 1;

  template <typename ImageType, typename ScalarAccess = default_float_t, typename ScalarOutputElement = default_float_t>
  static auto interpolate(const ImageType& img, ScalarAccess x, ScalarAccess y) noexcept;

  template <typename DerivedSrc, typename ScalarAccess = default_float_t, typename ScalarOutputElement = default_float_t>
  static auto interpolate(const ImageBase<DerivedSrc>& img, ScalarAccess x, ScalarAccess y) noexcept;

  template <typename T,
            std::size_t nr_channels,
            PixelFormat pixel_format,
            ImageModifiability modifiability,
            typename ScalarAccess = default_float_t,
            typename ScalarOutputElement = default_float_t>
  static auto interpolate(const ImageView<Pixel<T, nr_channels, pixel_format>, modifiability>& img, ScalarAccess x, ScalarAccess y) noexcept;

  template <typename T,
      std::size_t nr_channels,
      PixelFormat pixel_format,
      typename ScalarAccess = default_float_t,
      typename ScalarOutputElement = default_float_t>
  static auto interpolate(const Image<Pixel<T, nr_channels, pixel_format>>& img, ScalarAccess x, ScalarAccess y) noexcept;
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
template <typename ImageType, typename ScalarAccess>
inline auto ImageInterpolator<ImageInterpolationMode::NearestNeighbor, AccessMode>::interpolate(
    const ImageType& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  const auto ix = round_half_down<PixelIndex::value_type>(x);
  const auto iy = round_half_down<PixelIndex::value_type>(y);
  return ImageBorderAccessor<AccessMode>::access(img, PixelIndex{ix}, PixelIndex{iy});
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::NearestNeighbor` and the specified `BorderAccessMode`.
 *
 * (This explicit overload is necessary for the `RelativeAccessor<>` as given input, to apply any rounding after
 * conversion to absolute image coordinates.)
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam PixelType The pixel type.
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @param img The image to access, in form of a RelativeAccessor<> wrapper.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`.
 */
template <BorderAccessMode AccessMode>
template <typename PixelType, typename ScalarAccess>
inline auto ImageInterpolator<ImageInterpolationMode::NearestNeighbor, AccessMode>::interpolate(
    const RelativeAccessor<PixelType>& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");

  const auto abs_xy = img.absolute_coordinates(x, y);
  static_assert(std::is_same<decltype(abs_xy.x), ScalarAccess>::value, "Type mismatch");
  static_assert(std::is_same<decltype(abs_xy.y), ScalarAccess>::value, "Type mismatch");

  const auto ix = round_half_down<PixelIndex::value_type>(abs_xy.x);
  const auto iy = round_half_down<PixelIndex::value_type>(abs_xy.y);
  return ImageBorderAccessor<AccessMode>::access(img.image(), PixelIndex{ix}, PixelIndex{iy});
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::Bilinear` and the specified `BorderAccessMode`.
 *
 * The underlying `PixelType` of the image needs to support addition, subtraction, and multiplication by a scalar value.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam ImageType The image type.
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @tparam ScalarOutputElement The floating point type for each pixel element; i.e. the type for each channel element.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`. Will be of type `PixelType`.
 */
template <BorderAccessMode AccessMode>
template <typename ImageType, typename ScalarAccess, typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(const ImageType& img,
                                                                                         ScalarAccess x,
                                                                                         ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  static_assert(std::is_floating_point<ScalarOutputElement>::value,
                "Output pixel channel values must be floating point.");

  const auto x_floor = static_cast<PixelIndex::value_type>(x);
  const auto y_floor = static_cast<PixelIndex::value_type>(y);

  const auto rx = ScalarOutputElement(x - x_floor);
  const auto ry = ScalarOutputElement(y - y_floor);

//  const auto dx = (rx == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
//  const auto dy = (ry == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
  constexpr auto dx = PixelIndex::value_type{1};
  constexpr auto dy = PixelIndex::value_type{1};

  const auto& a = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor +  0});
  const auto& b = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor +  0});
  const auto& c = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor + dy});
  const auto& d = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor + dy});

  const auto result = a + ((b - a) * rx) + ((c - a) * ry) + ((a - b - c + d) * rx * ry);
  return result;
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::Bilinear` and the specified `BorderAccessMode`.
 *
 * This is an overload for inputs of type `ImageBase<Derived>`; it will delegate to the respective `ImageView` or
 * `Image` overload.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam DerivedSrc The derived image type (CRTP).
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @tparam ScalarOutputElement The floating point type for each pixel element; i.e. the type for each channel element.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`. Will be of type `PixelType`.
 */
template <BorderAccessMode AccessMode>
template <typename DerivedSrc, typename ScalarAccess, typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(const ImageBase<DerivedSrc>& img,
                                                                                         ScalarAccess x,
                                                                                         ScalarAccess y) noexcept
{
  return ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(img.derived(), x, y);
}

/** \brief Accesses the pixel value of `img` at floating point location (x, y) using the interpolation mode
 * `ImageInterpolationMode::Bilinear` and the specified `BorderAccessMode`.
 *
 * This is an overload for images `ImageView<PixelType, modifiability>`, where `PixelType` is `Pixel<T, nr_channels>`.
 *
 * @tparam AccessMode The border access mode (`BorderAccessMode`) to use.
 * @tparam T The pixel element type; i.e. the type of each channel element.
 * @tparam nr_channels The number of channels for a pixel.
 * @tparam modifiability The modifiability state of the image elements (mutable or constant).
 * @tparam ScalarAccess The floating point type for specifying the location (x, y).
 * @tparam ScalarOutputElement The floating point type for each pixel element; i.e. the type for each channel element.
 * @param img The image to access.
 * @param x The x-coordinate.
 * @param y The y-coordinate.
 * @return The pixel value at (x, y), using `ImageInterpolationMode::NearestNeighbor`. Will be of type
 * `Pixel<ScalarOutputElement, nr_channels>`.
 */
template <BorderAccessMode AccessMode>
template <typename T, std::size_t nr_channels, PixelFormat pixel_format, ImageModifiability modifiability,
    typename ScalarAccess, typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(
    const ImageView<Pixel<T, nr_channels, pixel_format>, modifiability>& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  static_assert(std::is_floating_point<ScalarOutputElement>::value,
                "Output pixel channel values must be floating point.");

  const auto x_floor = static_cast<PixelIndex::value_type>(x);
  const auto y_floor = static_cast<PixelIndex::value_type>(y);

  const auto rx = ScalarOutputElement(x - x_floor);
  const auto ry = ScalarOutputElement(y - y_floor);

//  const auto dx = (rx == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
//  const auto dy = (ry == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
  constexpr auto dx = PixelIndex::value_type{1};
  constexpr auto dy = PixelIndex::value_type{1};

  const auto& a = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor +  0});
  const auto& b = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor +  0});
  const auto& c = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor + dy});
  const auto& d = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor + dy});

  Pixel<ScalarOutputElement, nr_channels, pixel_format> dst;
  for (std::size_t i = 0; i < nr_channels; ++i)  // nr_channels is known at compile-time
  {
    dst[i] = ScalarOutputElement(a[i]) + ((b[i] - a[i]) * rx) + ((c[i] - a[i]) * ry)
             + ((a[i] - b[i] - c[i] + d[i]) * rx * ry);
  }
  return dst;
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
 * @return The pixel value at (x, y), using `ImageInterpolationMode::Bilinear`. Will be of type
 * `Pixel<ScalarOutputElement, nr_channels>`.
 */
template <BorderAccessMode AccessMode>
template <typename T, std::size_t nr_channels, PixelFormat pixel_format, typename ScalarAccess,
          typename ScalarOutputElement>
inline auto ImageInterpolator<ImageInterpolationMode::Bilinear, AccessMode>::interpolate(
    const Image<Pixel<T, nr_channels, pixel_format>>& img, ScalarAccess x, ScalarAccess y) noexcept
{
  static_assert(std::is_floating_point<ScalarAccess>::value, "Interpolation coordinates must be floating point.");
  static_assert(std::is_floating_point<ScalarOutputElement>::value,
                "Output pixel channel values must be floating point.");

  const auto x_floor = static_cast<PixelIndex::value_type>(x);
  const auto y_floor = static_cast<PixelIndex::value_type>(y);

  const auto rx = ScalarOutputElement(x - x_floor);
  const auto ry = ScalarOutputElement(y - y_floor);

//  const auto dx = (rx == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
//  const auto dy = (ry == 0) ? PixelIndex::value_type{0} : PixelIndex::value_type{1};
  constexpr auto dx = PixelIndex::value_type{1};
  constexpr auto dy = PixelIndex::value_type{1};

  const auto& a = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor +  0});
  const auto& b = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor +  0});
  const auto& c = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor +  0}, PixelIndex{y_floor + dy});
  const auto& d = ImageBorderAccessor<AccessMode>::access(img, PixelIndex{x_floor + dx}, PixelIndex{y_floor + dy});

  Pixel<ScalarOutputElement, nr_channels, pixel_format> dst;
  for (std::size_t i = 0; i < nr_channels; ++i)  // nr_channels is known at compile-time
  {
    dst[i] = ScalarOutputElement(a[i]) + ((b[i] - a[i]) * rx) + ((c[i] - a[i]) * ry)
             + ((a[i] - b[i] - c[i] + d[i]) * rx * ry);
  }
  return dst;
}

}  // namespace sln

#endif  // SELENE_IMG_INTERPOLATORS_HPP
