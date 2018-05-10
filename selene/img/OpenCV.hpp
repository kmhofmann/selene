// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_OPENCV_HPP
#define SELENE_IMG_OPENCV_HPP

/// @file

#if defined(SELENE_WITH_OPENCV)

#include <selene/base/Types.hpp>

#include <selene/img/Image.hpp>
#include <selene/img/Pixel.hpp>

#include <opencv2/core/core.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>

namespace sln {

template <typename T>
Image<T> wrap_opencv_mat(cv::Mat& img_cv);

template <typename T>
Image<T> copy_opencv_mat(const cv::Mat& img_cv);

template <typename T>
cv::Mat wrap_in_opencv_mat(Image<T>& img);

template <typename T>
cv::Mat copy_to_opencv_mat(const Image<T>& img);

// ----------
// Implementation:

namespace detail {

// clang-format off
template <typename T> struct PixelToOpenCVType;

template <> struct PixelToOpenCVType<std::uint8_t>{ static constexpr auto type = CV_8UC1; };
template <> struct PixelToOpenCVType<Pixel<std::uint8_t, 1>>{ static constexpr auto type = CV_8UC1; };
template <> struct PixelToOpenCVType<Pixel<std::uint8_t, 2>>{ static constexpr auto type = CV_8UC2; };
template <> struct PixelToOpenCVType<Pixel<std::uint8_t, 3>>{ static constexpr auto type = CV_8UC3; };
template <> struct PixelToOpenCVType<Pixel<std::uint8_t, 4>>{ static constexpr auto type = CV_8UC4; };

template <> struct PixelToOpenCVType<std::int8_t>{ static constexpr auto type = CV_8SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int8_t, 1>>{ static constexpr auto type = CV_8SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int8_t, 2>>{ static constexpr auto type = CV_8SC2; };
template <> struct PixelToOpenCVType<Pixel<std::int8_t, 3>>{ static constexpr auto type = CV_8SC3; };
template <> struct PixelToOpenCVType<Pixel<std::int8_t, 4>>{ static constexpr auto type = CV_8SC4; };

template <> struct PixelToOpenCVType<std::uint16_t>{ static constexpr auto type = CV_16UC1; };
template <> struct PixelToOpenCVType<Pixel<std::uint16_t, 1>>{ static constexpr auto type = CV_16UC1; };
template <> struct PixelToOpenCVType<Pixel<std::uint16_t, 2>>{ static constexpr auto type = CV_16UC2; };
template <> struct PixelToOpenCVType<Pixel<std::uint16_t, 3>>{ static constexpr auto type = CV_16UC3; };
template <> struct PixelToOpenCVType<Pixel<std::uint16_t, 4>>{ static constexpr auto type = CV_16UC4; };

template <> struct PixelToOpenCVType<std::int16_t>{ static constexpr auto type = CV_16SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int16_t, 1>>{ static constexpr auto type = CV_16SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int16_t, 2>>{ static constexpr auto type = CV_16SC2; };
template <> struct PixelToOpenCVType<Pixel<std::int16_t, 3>>{ static constexpr auto type = CV_16SC3; };
template <> struct PixelToOpenCVType<Pixel<std::int16_t, 4>>{ static constexpr auto type = CV_16SC4; };

template <> struct PixelToOpenCVType<std::int32_t>{ static constexpr auto type = CV_32SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int32_t, 1>>{ static constexpr auto type = CV_32SC1; };
template <> struct PixelToOpenCVType<Pixel<std::int32_t, 2>>{ static constexpr auto type = CV_32SC2; };
template <> struct PixelToOpenCVType<Pixel<std::int32_t, 3>>{ static constexpr auto type = CV_32SC3; };
template <> struct PixelToOpenCVType<Pixel<std::int32_t, 4>>{ static constexpr auto type = CV_32SC4; };

template <> struct PixelToOpenCVType<float32_t>{ static constexpr auto type = CV_32FC1; };
template <> struct PixelToOpenCVType<Pixel<float32_t, 1>>{ static constexpr auto type = CV_32FC1; };
template <> struct PixelToOpenCVType<Pixel<float32_t, 2>>{ static constexpr auto type = CV_32FC2; };
template <> struct PixelToOpenCVType<Pixel<float32_t, 3>>{ static constexpr auto type = CV_32FC3; };
template <> struct PixelToOpenCVType<Pixel<float32_t, 4>>{ static constexpr auto type = CV_32FC4; };

template <> struct PixelToOpenCVType<float64_t>{ static constexpr auto type = CV_64FC1; };
template <> struct PixelToOpenCVType<Pixel<float64_t, 1>>{ static constexpr auto type = CV_64FC1; };
template <> struct PixelToOpenCVType<Pixel<float64_t, 2>>{ static constexpr auto type = CV_64FC2; };
template <> struct PixelToOpenCVType<Pixel<float64_t, 3>>{ static constexpr auto type = CV_64FC3; };
template <> struct PixelToOpenCVType<Pixel<float64_t, 4>>{ static constexpr auto type = CV_64FC4; };
// clang-format on

inline int opencv_nr_bytes_per_channel(const cv::Mat& img_cv)
{
  switch (img_cv.depth())
  {
    case CV_8U:
    case CV_8S: return 1;
    case CV_16U:
    case CV_16S: return 2;
    case CV_32S:
    case CV_32F: return 4;
    case CV_64F: return 8;
    default: return 0;  // Unknown
  }
}

inline bool opencv_mat_type_is_floating_point(const cv::Mat& img_cv)
{
  return (img_cv.depth() == CV_32F || img_cv.depth() == CV_64F);
}

inline bool opencv_mat_type_is_integral(const cv::Mat& img_cv)
{
  return !(opencv_mat_type_is_floating_point(img_cv));
}

inline bool opencv_mat_type_is_unsigned(const cv::Mat& img_cv)
{
  return (img_cv.depth() == CV_8U || img_cv.depth() == CV_16U);
}

}  // namespace detail

/** \brief Wraps an OpenCV `cv::Mat` matrix instance in an `Image<T>`.
 *
 * The pixel type properties of type `T` (i.e. `PixelTraits<T>`) have to be compatible to the properties of the
 * `cv::Mat` instance. This is being checked only via debug mode assertions.
 *
 * @tparam T The pixel type of the `Image<>` to be returned.
 * @param img_cv An OpenCV matrix.
 * @return An `Image<T>`, providing a non-owning view on the OpenCV matrix `img_cv`.
 */
template <typename T>
inline Image<T> wrap_opencv_mat(cv::Mat& img_cv)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img_cv.step) > 0);

  SELENE_ASSERT(img_cv.channels() == PixelTraits<T>::nr_channels);
  SELENE_ASSERT(detail::opencv_nr_bytes_per_channel(img_cv) == PixelTraits<T>::nr_bytes_per_channel);
  SELENE_ASSERT(detail::opencv_mat_type_is_integral(img_cv) == PixelTraits<T>::is_integral);
  SELENE_ASSERT(detail::opencv_mat_type_is_floating_point(img_cv) == PixelTraits<T>::is_floating_point);
  SELENE_ASSERT(detail::opencv_mat_type_is_unsigned(img_cv) == PixelTraits<T>::is_unsigned);

  const auto data = img_cv.data;
  const auto width = PixelLength(static_cast<PixelLength::value_type>(img_cv.cols));
  const auto height = PixelLength(static_cast<PixelLength::value_type>(img_cv.rows));
  const auto stride_bytes = Stride(img_cv.step);
  return Image<T>(data, width, height, stride_bytes);
}

/** \brief Copies an OpenCV `cv::Mat` matrix instance to an `Image<T>`.
 *
 * The pixel type properties of type `T` (i.e. `PixelTraits<T>`) have to be compatible to the properties of the
 * `cv::Mat` instance. This is being checked only via debug mode assertions.
 *
 * @tparam T The pixel type of the `Image<>` to be returned.
 * @param img_cv An OpenCV matrix.
 * @return An `Image<T>`, providing a copy of the contents of the OpenCV matrix `img_cv`.
 */
template <typename T>
Image<T> copy_opencv_mat(const cv::Mat& img_cv)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img_cv.step) > 0);

  SELENE_ASSERT(img_cv.channels() == PixelTraits<T>::nr_channels);
  SELENE_ASSERT(detail::opencv_nr_bytes_per_channel(img_cv) == PixelTraits<T>::nr_bytes_per_channel);
  SELENE_ASSERT(detail::opencv_mat_type_is_integral(img_cv) == PixelTraits<T>::is_integral);
  SELENE_ASSERT(detail::opencv_mat_type_is_floating_point(img_cv) == PixelTraits<T>::is_floating_point);
  SELENE_ASSERT(detail::opencv_mat_type_is_unsigned(img_cv) == PixelTraits<T>::is_unsigned);

  const auto width = PixelLength(static_cast<PixelLength::value_type>(img_cv.cols));
  const auto height = PixelLength(static_cast<PixelLength::value_type>(img_cv.rows));
  const auto stride_bytes = Stride(img_cv.step);

  const auto nr_bytes_per_row = width * PixelTraits<T>::nr_bytes;

  Image<T> img(width, height, stride_bytes);
  for (PixelIndex y = 0_px; y < height; ++y)
  {
    const auto begin = img_cv.ptr(y);
    const auto end = img_cv.ptr(y) + nr_bytes_per_row;
    std::copy(begin, end, img.byte_ptr(y));
  }
  return img;
}

/** \brief Wraps an `Image<T>` in an OpenCV cv::Mat.
 *
 * @tparam T The pixel type of the `Image<>`.
 * @param img An image.
 * @return A `cv::Mat`, providing a non-owning view on the data of `img`.
 */
template <typename T>
inline cv::Mat wrap_in_opencv_mat(Image<T>& img)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img.width()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(static_cast<std::int64_t>(img.height()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(img.stride_bytes() <= std::numeric_limits<std::size_t>::max());

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();

  const auto type = detail::PixelToOpenCVType<T>::type;
  const auto data = static_cast<void*>(img.byte_ptr());

  return cv::Mat(height, width, type, data, stride_bytes);
}

/** \brief Copies an `Image<T>` to an OpenCV cv::Mat.
 *
 * @tparam T The pixel type of the `Image<>`.
 * @param img An image.
 * @return A `cv::Mat`, providing a copy of the contents of `img`.
 */
template <typename T>
cv::Mat copy_to_opencv_mat(const Image<T>& img)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img.width()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(static_cast<std::int64_t>(img.height()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(img.stride_bytes() <= std::numeric_limits<std::size_t>::max());

  const auto width = img.width();
  const auto height = img.height();
  const auto nr_bytes_per_row = width * PixelTraits<T>::nr_bytes;

  const auto type = detail::PixelToOpenCVType<T>::type;
  cv::Mat img_cv(height, width, type);

  for (int row = 0; row < img_cv.rows; ++row)
  {
    const auto begin = img.byte_ptr(PixelIndex(row));
    const auto end = img.byte_ptr(PixelIndex(row)) + nr_bytes_per_row;
    std::copy(begin, end, img_cv.ptr(row));
  }

  return img_cv;
}

}  // namespace sln

#endif  // defined(SELENE_WITH_OPENCV)

#endif  // SELENE_IMG_OPENCV_HPP
