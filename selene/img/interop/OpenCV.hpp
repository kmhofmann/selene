// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_INTEROP_OPENCV_HPP
#define SELENE_IMG_INTEROP_OPENCV_HPP

/// @file

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_OPENCV)

#include <selene/img/pixel/Pixel.hpp>
#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageBase.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <opencv2/core.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>

namespace sln {

/// \addtogroup group-img-interop
/// @{

template <typename PixelType>
MutableImageView<PixelType> wrap_opencv_mat(cv::Mat& img_cv);

template <typename PixelType>
ConstantImageView<PixelType> wrap_opencv_mat(const cv::Mat& img_cv);

template <typename PixelType>
Image<PixelType> copy_opencv_mat(const cv::Mat& img_cv);

template <typename DerivedSrc>
cv::Mat wrap_in_opencv_mat(ImageBase<DerivedSrc>& img);

template <typename DerivedSrc>
cv::Mat copy_to_opencv_mat(const ImageBase<DerivedSrc>& img);

/// @}

// ----------
// Implementation:

namespace impl {

// clang-format off
template <typename T> struct PixelToOpenCVType;

template <> struct PixelToOpenCVType<std::uint8_t>{ static constexpr auto type = CV_8UC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint8_t, 1, pixel_format>>{ static constexpr auto type = CV_8UC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint8_t, 2, pixel_format>>{ static constexpr auto type = CV_8UC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint8_t, 3, pixel_format>>{ static constexpr auto type = CV_8UC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint8_t, 4, pixel_format>>{ static constexpr auto type = CV_8UC4; };

template <> struct PixelToOpenCVType<std::int8_t>{ static constexpr auto type = CV_8SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int8_t, 1, pixel_format>>{ static constexpr auto type = CV_8SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int8_t, 2, pixel_format>>{ static constexpr auto type = CV_8SC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int8_t, 3, pixel_format>>{ static constexpr auto type = CV_8SC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int8_t, 4, pixel_format>>{ static constexpr auto type = CV_8SC4; };

template <> struct PixelToOpenCVType<std::uint16_t>{ static constexpr auto type = CV_16UC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint16_t, 1, pixel_format>>{ static constexpr auto type = CV_16UC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint16_t, 2, pixel_format>>{ static constexpr auto type = CV_16UC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint16_t, 3, pixel_format>>{ static constexpr auto type = CV_16UC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::uint16_t, 4, pixel_format>>{ static constexpr auto type = CV_16UC4; };

template <> struct PixelToOpenCVType<std::int16_t>{ static constexpr auto type = CV_16SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int16_t, 1, pixel_format>>{ static constexpr auto type = CV_16SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int16_t, 2, pixel_format>>{ static constexpr auto type = CV_16SC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int16_t, 3, pixel_format>>{ static constexpr auto type = CV_16SC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int16_t, 4, pixel_format>>{ static constexpr auto type = CV_16SC4; };

template <> struct PixelToOpenCVType<std::int32_t>{ static constexpr auto type = CV_32SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int32_t, 1, pixel_format>>{ static constexpr auto type = CV_32SC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int32_t, 2, pixel_format>>{ static constexpr auto type = CV_32SC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int32_t, 3, pixel_format>>{ static constexpr auto type = CV_32SC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<std::int32_t, 4, pixel_format>>{ static constexpr auto type = CV_32SC4; };

template <> struct PixelToOpenCVType<float32_t>{ static constexpr auto type = CV_32FC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float32_t, 1, pixel_format>>{ static constexpr auto type = CV_32FC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float32_t, 2, pixel_format>>{ static constexpr auto type = CV_32FC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float32_t, 3, pixel_format>>{ static constexpr auto type = CV_32FC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float32_t, 4, pixel_format>>{ static constexpr auto type = CV_32FC4; };

template <> struct PixelToOpenCVType<float64_t>{ static constexpr auto type = CV_64FC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float64_t, 1, pixel_format>>{ static constexpr auto type = CV_64FC1; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float64_t, 2, pixel_format>>{ static constexpr auto type = CV_64FC2; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float64_t, 3, pixel_format>>{ static constexpr auto type = CV_64FC3; };
template <PixelFormat pixel_format> struct PixelToOpenCVType<Pixel<float64_t, 4, pixel_format>>{ static constexpr auto type = CV_64FC4; };
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

}  // namespace impl

/** \brief Wraps an OpenCV `cv::Mat` matrix instance in an `Image<PixelType>`.
 *
 * The pixel type properties of type `PixelType` (i.e. `PixelTraits<PixelType>`) have to be compatible to the
 * properties of the `cv::Mat` instance. This is being checked only via debug mode assertions.
 *
 * @tparam PixelType The pixel type of the `Image<>` to be returned.
 * @param img_cv An OpenCV matrix.
 * @return A `MutableImageView<PixelType>`, providing a non-owning view on the OpenCV matrix `img_cv`.
 */
template <typename PixelType>
inline MutableImageView<PixelType> wrap_opencv_mat(cv::Mat& img_cv)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img_cv.step) > 0);

  SELENE_ASSERT(img_cv.channels() == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(impl::opencv_nr_bytes_per_channel(img_cv) == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(impl::opencv_mat_type_is_integral(img_cv) == PixelTraits<PixelType>::is_integral);
  SELENE_ASSERT(impl::opencv_mat_type_is_floating_point(img_cv) == PixelTraits<PixelType>::is_floating_point);
  SELENE_ASSERT(impl::opencv_mat_type_is_unsigned(img_cv) == PixelTraits<PixelType>::is_unsigned);

  const auto data = img_cv.data;
  const auto width = to_pixel_length(img_cv.cols);
  const auto height = to_pixel_length(img_cv.rows);
  const auto stride_bytes = to_stride(img_cv.step);
  return MutableImageView<PixelType>(data, {width, height, stride_bytes});
}

/** \brief Wraps an OpenCV `cv::Mat` matrix instance in an `Image<PixelType>`.
 *
 * The pixel type properties of type `PixelType` (i.e. `PixelTraits<PixelType>`) have to be compatible to the
 * properties of the `cv::Mat` instance. This is being checked only via debug mode assertions.
 *
 * @tparam PixelType The pixel type of the `Image<>` to be returned.
 * @param img_cv An OpenCV matrix.
 * @return A `ConstantImageView<PixelType>`, providing a non-owning view on the OpenCV matrix `img_cv`.
 */
template <typename PixelType>
inline ConstantImageView<PixelType> wrap_opencv_mat(const cv::Mat& img_cv)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img_cv.step) > 0);

  SELENE_ASSERT(img_cv.channels() == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(impl::opencv_nr_bytes_per_channel(img_cv) == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(impl::opencv_mat_type_is_integral(img_cv) == PixelTraits<PixelType>::is_integral);
  SELENE_ASSERT(impl::opencv_mat_type_is_floating_point(img_cv) == PixelTraits<PixelType>::is_floating_point);
  SELENE_ASSERT(impl::opencv_mat_type_is_unsigned(img_cv) == PixelTraits<PixelType>::is_unsigned);

  const auto data = img_cv.data;
  const auto width = to_pixel_length(img_cv.cols);
  const auto height = to_pixel_length(img_cv.rows);
  const auto stride_bytes = to_stride(img_cv.step);
  return ConstantImageView<PixelType>(data, {width, height, stride_bytes});
}

/** \brief Copies an OpenCV `cv::Mat` matrix instance to an `Image<PixelType>`.
 *
 * The pixel type properties of type `PixelType` (i.e. `PixelTraits<PixelType>`) have to be compatible to the
 * properties of the `cv::Mat` instance. This is being checked only via debug mode assertions.
 *
 * @tparam T The pixel type of the `Image<>` to be returned.
 * @param img_cv An OpenCV matrix.
 * @return An `Image<PixelType>`, providing a copy of the contents of the OpenCV matrix `img_cv`.
 */
template <typename PixelType>
Image<PixelType> copy_opencv_mat(const cv::Mat& img_cv)
{
  SELENE_ASSERT(static_cast<std::int64_t>(img_cv.step) > 0);

  SELENE_ASSERT(img_cv.channels() == PixelTraits<PixelType>::nr_channels);
  SELENE_ASSERT(impl::opencv_nr_bytes_per_channel(img_cv) == PixelTraits<PixelType>::nr_bytes_per_channel);
  SELENE_ASSERT(impl::opencv_mat_type_is_integral(img_cv) == PixelTraits<PixelType>::is_integral);
  SELENE_ASSERT(impl::opencv_mat_type_is_floating_point(img_cv) == PixelTraits<PixelType>::is_floating_point);
  SELENE_ASSERT(impl::opencv_mat_type_is_unsigned(img_cv) == PixelTraits<PixelType>::is_unsigned);

  const auto width = to_pixel_length(img_cv.cols);
  const auto height = to_pixel_length(img_cv.rows);
  const auto stride_bytes = to_stride(img_cv.step);

  const auto nr_bytes_per_row = width * PixelTraits<PixelType>::nr_bytes;

  Image<PixelType> img({width, height, stride_bytes});
  for (PixelIndex y = 0_idx; y < height; ++y)
  {
    const auto begin = img_cv.ptr(y);
    const auto end = img_cv.ptr(y) + nr_bytes_per_row;
    std::copy(begin, end, img.byte_ptr(y));
  }
  return img;
}

/** \brief Wraps an `Image<PixelType>` in an OpenCV cv::Mat.
 *
 * @tparam DerivedSrc The typed source image type.
 * @param img An image.
 * @return A `cv::Mat`, providing a non-owning view on the data of `img`.
 */
template <typename DerivedSrc>
inline cv::Mat wrap_in_opencv_mat(ImageBase<DerivedSrc>& img)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;

  SELENE_ASSERT(static_cast<std::int64_t>(img.width()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(static_cast<std::int64_t>(img.height()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(img.stride_bytes() <= std::numeric_limits<std::ptrdiff_t>::max());

  const auto width = img.width();
  const auto height = img.height();
  const auto stride_bytes = img.stride_bytes();

  const auto type = impl::PixelToOpenCVType<PixelType>::type;
  const auto data = static_cast<void*>(img.byte_ptr());

  return cv::Mat(height, width, type, data, static_cast<std::size_t>(stride_bytes));
}

/** \brief Copies an `Image<PixelType>` to an OpenCV cv::Mat.
 *
 * @tparam DerivedSrc The typed source image type.
 * @param img An image.
 * @return A `cv::Mat`, providing a copy of the contents of `img`.
 */
template <typename DerivedSrc>
cv::Mat copy_to_opencv_mat(const ImageBase<DerivedSrc>& img)
{
  using PixelType = typename ImageBase<DerivedSrc>::PixelType;

  SELENE_ASSERT(static_cast<std::int64_t>(img.width()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(static_cast<std::int64_t>(img.height()) <= static_cast<std::int64_t>(std::numeric_limits<int>::max()));
  SELENE_ASSERT(img.stride_bytes() <= std::numeric_limits<std::ptrdiff_t>::max());

  const auto width = img.width();
  const auto height = img.height();
  const auto nr_bytes_per_row = img.row_bytes();

  const auto type = impl::PixelToOpenCVType<PixelType>::type;
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

#endif  // defined(SELENE_IMG_INTEROP_OPENCV_HPP)

#endif  // SELENE_IMG_OPENCV_HPP
