// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#if defined(SELENE_WITH_OPENCV)

#include <selene/img/Pixel.hpp>
#include <selene/img/Image.hpp>
#include <selene/img/OpenCV.hpp>

using namespace selene;
using namespace selene::img;

namespace {

template <typename PixelType> struct PixelProducer;

template <typename T> struct PixelProducer<Pixel<T, 1>>
{
  static Pixel<T, 1> get(Index x, Index y) { return Pixel<T, 1>(x + y); }
};

template <typename T> struct PixelProducer<Pixel<T, 2>>
{
  static Pixel<T, 2> get(Index x, Index y) { return Pixel<T, 2>(x + y, 2 * x + y); }
};

template <typename T> struct PixelProducer<Pixel<T, 3>>
{
  static Pixel<T, 3> get(Index x, Index y) { return Pixel<T, 3>(x + y, 2 * x + y, x + 2 * y); }
};

template <typename T> struct PixelProducer<Pixel<T, 4>>
{
  static Pixel<T, 4> get(Index x, Index y) { return Pixel<T, 4>(x + y, 2 * x + y, x + 2 * y, 2 * x + 2 * y); }
};

template <typename PixelType> Image<PixelType> create_test_image(Length width, Length height)
{
  Image<PixelType> img(width, height);

  for (Index y = 0; y < img.height(); ++y)
  {
    for (Index x = 0; x < img.width(); ++x)
    {
      img(x, y) = PixelProducer<PixelType>::get(x, y);
    }
  }

  return img;
}

template <typename PixelType> cv::Mat create_test_image_cv(Length width, Length height)
{
  using Element = typename PixelTraits<PixelType>::Element;
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;

  const auto rows = static_cast<int>(height);
  const auto cols = static_cast<int>(width);
  const auto type = img::detail::PixelToOpenCVType<PixelType>::type;
  cv::Mat img_cv(rows, cols, type);

  for (int row = 0; row < img_cv.rows; ++row)
  {
    for (int col = 0; col < img_cv.cols; ++col)
    {
      const auto pixel = PixelProducer<PixelType>::get(col, row);
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

template <typename PixelType> void compare_images(const Image<PixelType>& img, const cv::Mat& img_cv)
{
  using Element = typename PixelTraits<PixelType>::Element;
  constexpr auto nr_channels = PixelTraits<PixelType>::nr_channels;
  constexpr auto nr_bytes = PixelTraits<PixelType>::nr_bytes;
  constexpr auto nr_bytes_per_channel = PixelTraits<PixelType>::nr_bytes_per_channel;

  REQUIRE(img_cv.channels() == nr_channels);
  REQUIRE(img::detail::opencv_nr_bytes_per_channel(img_cv) == nr_bytes_per_channel);
  REQUIRE(img_cv.cols == static_cast<int>(img.width()));
  REQUIRE(img_cv.rows == static_cast<int>(img.height()));

  for (int row = 0; row < img_cv.rows; ++row)
  {
    for (int col = 0; col < img_cv.cols; ++col)
    {
      const auto pixel = img(col, row);

      for (std::uint32_t channel = 0; channel < nr_channels; ++channel)
      {
        const auto element_cv = img_cv.at<Element>(row, col);
        REQUIRE(element_cv == pixel[channel]);
      }
    }
  }
}

template <typename PixelType> void test_functions(Length width, Length height)
{
  auto img = create_test_image<PixelType>(width, height);
  auto img_cv = create_test_image_cv<PixelType>(width, height);

  const auto img_wrapped = wrap_opencv_mat<PixelType>(img_cv);
  compare_images(img_wrapped, img_cv);

  const auto img_copied = copy_opencv_mat<PixelType>(img_cv);
  compare_images(img_copied, img_cv);

  const auto img_cv_wrapped = wrap_in_opencv_mat(img);
  compare_images(img, img_cv_wrapped);

  const auto img_cv_copied = copy_to_opencv_mat(img);
  compare_images(img, img_cv_copied);
}

} // namespace _

TEST_CASE("OpenCV interoperability", "[img]")
{
  for (Length w = 1; w < 32; w += 1)
  {
    for (Length h = 1; h < 32; h += 1)
    {
      test_functions<Pixel_8u1>(w, h);
    }
  }
}

#endif // defined(SELENE_WITH_OPENCV)
