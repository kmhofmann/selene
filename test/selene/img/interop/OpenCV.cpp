// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#if defined(SELENE_WITH_OPENCV)

#include <selene/img/interop/OpenCV.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

using namespace sln::literals;

namespace {

template <typename PixelType>
struct PixelProducer;

template <typename T>
struct PixelProducer<sln::Pixel<T, 1>>
{
  static sln::Pixel<T, 1> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 1>(x + y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 2>>
{
  static sln::Pixel<T, 2> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 2>(x + y, 2 * x + y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 3>>
{
  static sln::Pixel<T, 3> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 3>(x + y, 2 * x + y, x + 2 * y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 4>>
{
  static sln::Pixel<T, 4> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 4>(x + y, 2 * x + y, x + 2 * y, 2 * x + 2 * y);
  }
};

template <typename PixelType>
sln::Image<PixelType> create_test_image(sln::PixelLength width, sln::PixelLength height)
{
  sln::Image<PixelType> img({width, height});

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      img(x, y) = PixelProducer<PixelType>::get(x, y);
    }
  }

  return img;
}

template <typename PixelType>
cv::Mat create_test_image_cv(sln::PixelLength width, sln::PixelLength height)
{
  using Element = typename sln::PixelTraits<PixelType>::Element;
  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;

  const auto rows = static_cast<int>(height);
  const auto cols = static_cast<int>(width);
  const auto type = sln::impl::PixelToOpenCVType<PixelType>::type;
  cv::Mat img_cv(rows, cols, type);

  for (int row = 0; row < img_cv.rows; ++row)
  {
    for (int col = 0; col < img_cv.cols; ++col)
    {
      const auto pixel = PixelProducer<PixelType>::get(sln::PixelIndex(col), sln::PixelIndex(row));
      cv::Vec<Element, nr_channels> pixel_cv;

      for (int channel = 0; channel < img_cv.channels(); ++channel)
      {
        Element el = pixel[channel];
        pixel_cv[channel] = el;
      }

      img_cv.at<cv::Vec<Element, nr_channels>>(row, col) = pixel_cv;
    }
  }

  return img_cv;
}

template <typename DerivedSrc>
void compare_images(const sln::ImageBase<DerivedSrc>& img, const cv::Mat& img_cv)
{
  using PixelType = typename sln::ImageBase<DerivedSrc>::PixelType;

  using Element = typename sln::PixelTraits<PixelType>::Element;
  constexpr auto nr_channels = sln::PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes_per_channel = sln::PixelTraits<PixelType>::nr_bytes_per_channel;

  REQUIRE(img_cv.channels() == nr_channels);
  REQUIRE(sln::impl::opencv_nr_bytes_per_channel(img_cv) == nr_bytes_per_channel);
  REQUIRE(img_cv.cols == static_cast<int>(img.width()));
  REQUIRE(img_cv.rows == static_cast<int>(img.height()));

  for (int row = 0; row < img_cv.rows; ++row)
  {
    for (int col = 0; col < img_cv.cols; ++col)
    {
      const auto pixel = img(sln::PixelIndex(col), sln::PixelIndex(row));

      for (std::uint32_t channel = 0; channel < nr_channels; ++channel)
      {
        const auto element_cv = img_cv.at<cv::Vec<Element, nr_channels>>(row, col)[channel];
        REQUIRE(element_cv == pixel[channel]);
      }
    }
  }
}

template <typename PixelType>
void test_functions(sln::PixelLength width, sln::PixelLength height)
{
  auto img = create_test_image<PixelType>(width, height);
  auto img_cv = create_test_image_cv<PixelType>(width, height);
  const auto img_cv_c = create_test_image_cv<PixelType>(width, height);

  const auto img_wrapped = sln::wrap_opencv_mat<PixelType>(img_cv);
  compare_images(img_wrapped, img_cv);

  const auto img_wrapped_c = sln::wrap_opencv_mat<PixelType>(img_cv_c);
  compare_images(img_wrapped_c, img_cv);

  const auto img_copied = sln::copy_opencv_mat<PixelType>(img_cv);
  compare_images(img_copied, img_cv);

  const auto img_cv_wrapped = sln::wrap_in_opencv_mat(img);
  compare_images(img, img_cv_wrapped);

  const auto img_cv_copied = sln::copy_to_opencv_mat(img);
  compare_images(img, img_cv_copied);
}

}  // namespace

TEST_CASE("OpenCV interoperability", "[img]")
{
  for (auto w = 1_px; w < 32; w += 3)
  {
    for (auto h = 1_px; h < 32; h += 3)
    {
      test_functions<sln::Pixel_8u1>(w, h);
      test_functions<sln::Pixel_8u2>(w, h);
      test_functions<sln::Pixel_8u3>(w, h);
      test_functions<sln::Pixel_8u4>(w, h);
    }
  }
}

#endif  // defined(SELENE_WITH_OPENCV)
