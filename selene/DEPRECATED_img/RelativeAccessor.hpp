// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_RELATIVE_ACCESSOR_HPP
#define SELENE_IMG_RELATIVE_ACCESSOR_HPP

/// @file

#include <selene/DEPRECATED_img/Image.hpp>
#include <selene/DEPRECATED_img/Types.hpp>

namespace sln {

/** \brief Wraps an `Image<>` instance to provide coordinate access relative to an anchor point.
 *
 * Usually not explicitly created, but returned by the convenience function `relative_accesor`.
 *
 * An instance of `RelativeAccessor<>` is accepted in place of an `Image<>` instance in calls to
 * `ImageBorderAccessor<>::access`, `ImageInterpolator<>::access`, and the various forms of `get()` for pixel access.
 *
 * @tparam ImageType_ The image type.
 */
template <typename ImageType_>
class RelativeAccessor
{
public:
  using ImageType = ImageType_;  ///< The image type of the underlying image.
  using PixelType = typename ImageType::PixelType;  ///< The pixel type of the underlying image.

  /** \brief x/y coordinate.
   *
   * @tparam T The coordinate element type.
   */
  template <typename T = PixelIndex>
  struct XY
  {
    T x;  ///< X-coordinate.
    T y;  ///< Y-coordinate.
  };

  RelativeAccessor(ImageType& img, PixelIndex anchor_x, PixelIndex anchor_y);

  RelativeAccessor(const RelativeAccessor<ImageType_>&) noexcept = default;  ///< Copy constructor.
  RelativeAccessor<ImageType_>& operator=(const RelativeAccessor<ImageType_>&) noexcept = default;  ///< Copy assignment operator.

  RelativeAccessor(RelativeAccessor<ImageType_>&&) noexcept = default;  ///< Move constructor.
  RelativeAccessor<ImageType_>& operator=(RelativeAccessor<ImageType_>&&) noexcept = default;  ///< Move assignment operator.

  const ImageType& image() const noexcept;
  ImageType& image() noexcept;

  PixelIndex anchor_x() const noexcept;
  PixelIndex anchor_y() const noexcept;

  template <typename T>
  XY<T> absolute_coordinates(T x, T y) const noexcept;

  const PixelType& get(PixelIndex x, PixelIndex y) const noexcept;
  PixelType& get(PixelIndex x, PixelIndex y) noexcept;

private:
  ImageType_& img_;
  PixelIndex anchor_x_;
  PixelIndex anchor_y_;
};

template <typename ImageType>
auto relative_accessor(ImageType& img, PixelIndex anchor_x, PixelIndex anchor_y);


// ----------
// Implementation

/** \brief Constructor.
 *
 * @tparam ImageType_ The image type.
 * @param img The image to access with relative coordinates.
 * @param anchor_x The x-coordinate of the relative access coordinate origin.
 * @param anchor_y The y-coordinate of the relative access coordinate origin.
 */
template <typename ImageType_>
inline
RelativeAccessor<ImageType_>::RelativeAccessor(ImageType_& img, PixelIndex anchor_x, PixelIndex anchor_y)
: img_(img), anchor_x_(anchor_x), anchor_y_(anchor_y)
{
}

/** \brief Returns the underlying image.
 *
 * @tparam ImageType_ The image type.
 * @return The underlying image.
 */
template <typename ImageType_>
inline const ImageType_& RelativeAccessor<ImageType_>::image() const noexcept
{
  return img_;
}

/** \brief Returns the underlying image.
 *
 * @tparam ImageType_ The image type.
 * @return The underlying image.
 */
template <typename ImageType_>
inline ImageType_& RelativeAccessor<ImageType_>::image() noexcept
{
  return img_;
}

/** \brief Returns the x-coordinate of the relative coordinate origin.
 *
 * @tparam ImageType_ The image type.
 * @return X-coordinate of the relative coordinate origin.
 */
template <typename ImageType_>
inline PixelIndex RelativeAccessor<ImageType_>::anchor_x() const noexcept
{
  return anchor_x_;
}

/** \brief Returns the x-coordinate of the relative coordinate origin.
 *
 * @tparam ImageType_ The image type.
 * @return X-coordinate of the relative coordinate origin.
 */
template <typename ImageType_>
inline PixelIndex RelativeAccessor<ImageType_>::anchor_y() const noexcept
{
  return anchor_y_;
}

/** \brief Converts relative coordinates to absolute coordinates.
 *
 * @tparam ImageType_ The image type.
 * @tparam T The coordinate type.
 * @param x The relative x-coordinate.
 * @param y The relative y-coordinate.
 * @return The absolute coordinate pair corresponding to the supplied relative coordinates.
 */
template <typename ImageType_>
template <typename T>
#ifndef _MSC_VER
inline typename RelativeAccessor<ImageType_>::template XY<T>
#else
inline typename RelativeAccessor<ImageType_>::XY<T>
#endif
RelativeAccessor<ImageType_>::absolute_coordinates(T x, T y) const noexcept
{
  const auto abs_x = T{anchor_x_ + x};
  const auto abs_y = T{anchor_y_ + y};
  return {abs_x, abs_y};
}

/** \brief Accesses an image pixel with relative coordinates.
 *
 * No bounds checking is performed.
 * For the read-only case, this can be achieved by accessing the `RelativeImageAccessor<>` through the
 * `ImageBorderAccessor` interface, or through the `get()` accessor function.
 *
 * @tparam ImageType_ The image type.
 * @param x The relative x-coordinate.
 * @param y The relative y-coordinate.
 * @return The underlying image pixel.
 */
template <typename ImageType_>
inline const typename RelativeAccessor<ImageType_>::PixelType&
RelativeAccessor<ImageType_>::get(PixelIndex x, PixelIndex y) const noexcept
{
const auto abs_x = PixelIndex{anchor_x_ + x};
const auto abs_y = PixelIndex{anchor_y_ + y};
return img_(to_pixel_index(abs_x), to_pixel_index(abs_y));
}

/** \brief Accesses an image pixel with relative coordinates.
 *
 * No bounds checking is performed.
 *
 * @tparam ImageType_ The image type.
 * @param x The relative x-coordinate.
 * @param y The relative y-coordinate.
 * @return The underlying image pixel.
 */
template <typename ImageType_>
inline typename RelativeAccessor<ImageType_>::PixelType&
RelativeAccessor<ImageType_>::get(PixelIndex x, PixelIndex y) noexcept
{
  const auto abs_x = PixelIndex{anchor_x_ + x};
  const auto abs_y = PixelIndex{anchor_y_ + y};
  return img_(to_pixel_index(abs_x), to_pixel_index(abs_y));
}


/** \brief Returns a `RelativeAccessor<PixelType>` wrapper for an image.
 *
 * @tparam ImageType The image type.
 * @param img The image to access with relative coordinates.
 * @param anchor_x The x-coordinate of the relative coordinate origin.
 * @param anchor_y The y-coordinate of the relative coordinate origin.
 * @return A `RelativeAccessor<PixelType>` instance.
 */
template <typename ImageType>
inline auto relative_accessor(ImageType& img, PixelIndex anchor_x, PixelIndex anchor_y)
{
  return RelativeAccessor<ImageType>(img, anchor_x, anchor_y);
}

}  // namespace sln

#endif  // SELENE_IMG_RELATIVE_ACCESSOR_HPP
