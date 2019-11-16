// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/Assert.hpp>
#include <selene/base/Kernel.hpp>
#include <selene/base/io/FileReader.hpp>

#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/interop/OpenCV.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/access/BorderAccessors.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Convolution.hpp>
#include <selene/img_ops/ImageConversions.hpp>
#include <selene/img_ops/View.hpp>

#include <test/utils/Utils.hpp>

#include <benchmark/benchmark.h>

#if defined(SELENE_WITH_OPENCV)
#include <opencv2/imgproc.hpp>
#endif  // SELENE_IMG_OPENCV_HPP

#include <tuple>

using namespace sln::literals;

namespace {

template <sln::PixelFormat pixel_format_dst>
auto read_image(const std::string& filename)
{
  const auto full_path = sln_test::full_data_path(filename.c_str());
  auto dyn_img = sln::read_image(sln::FileReader(full_path.string()));
  SELENE_FORCED_ASSERT(dyn_img.is_valid());

  if constexpr (pixel_format_dst == sln::PixelFormat::RGB)
  {
    return sln::to_image<sln::PixelRGB_8u>(std::move(dyn_img));
  }
  else
  {
    auto img = sln::to_image<sln::PixelRGB_8u>(std::move(dyn_img));
    return sln::convert_image<sln::PixelFormat::Y>(img);
  }
}

template <sln::PixelFormat pixel_format_dst>
auto get_stuff()
{
  auto img = read_image<pixel_format_dst>("stickers.png");
  auto sub_view = sln::view(img, {20_idx, 20_idx, 64_px, 64_px});  // 64x64 pixel view
  auto kernel = sln::gaussian_kernel<7, double>(1.0);
  return std::tuple{std::move(img), sub_view, kernel};
}

}  // namespace _

template <sln::PixelFormat pixel_format_dst>
void image_convolution_x_floating_point_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  decltype(img) img_dst;

  for (auto _ : state)
  {
    sln::convolution_x<sln::BorderAccessMode::Unchecked>(sub_view, img_dst, kernel);
  }
}

template <sln::PixelFormat pixel_format_dst>
void image_convolution_y_floating_point_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  decltype(img) img_dst;

  for (auto _ : state)
  {
    sln::convolution_y<sln::BorderAccessMode::Unchecked>(sub_view, img_dst, kernel);
  }
}

template <sln::PixelFormat pixel_format_dst>
void image_convolution_x_integer_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  constexpr auto shift = 16u;
  const auto integer_kernel = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel);
  decltype(img) img_dst;

  for (auto _ : state)
  {
    sln::convolution_x<sln::BorderAccessMode::Unchecked, shift>(sub_view, img_dst, integer_kernel);
  }
}

template <sln::PixelFormat pixel_format_dst>
void image_convolution_y_integer_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  constexpr auto shift = 16u;
  const auto integer_kernel = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel);
  decltype(img) img_dst;

  for (auto _ : state)
  {
    sln::convolution_y<sln::BorderAccessMode::Unchecked, shift>(sub_view, img_dst, integer_kernel);
  }
}

#if defined(SELENE_WITH_OPENCV)

/* These functions use the more generic cv::filter2D function, and do not take into account the existence of a
 * potentially more optimized implementation in the cv::GaussianBlur function. The latter does not support
 * 1-D convolutions, but it does support in-place processing. */

template <sln::PixelFormat pixel_format_dst>
void image_convolution_x_opencv(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  cv::Mat sub_view_cv = sln::wrap_in_opencv_mat(sub_view);
  cv::Mat kernel_cv = cv::Mat(1, static_cast<int>(kernel.size()), CV_64FC1, &*kernel.begin(), sizeof(double) * kernel.size());
  cv::Mat img_dst_cv(sub_view_cv.rows, sub_view_cv.cols, sub_view_cv.type()); // pre-allocate

  for (auto _ : state)
  {
    cv::filter2D(sub_view_cv, img_dst_cv, -1, kernel_cv);
  }
}

template <sln::PixelFormat pixel_format_dst>
void image_convolution_y_opencv(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff<pixel_format_dst>();
  cv::Mat sub_view_cv = sln::wrap_in_opencv_mat(sub_view);
  cv::Mat kernel_cv = cv::Mat(static_cast<int>(kernel.size()), 1, CV_64FC1, &*kernel.begin(), sizeof(double) * kernel.size());
  cv::Mat img_dst_cv(sub_view_cv.rows, sub_view_cv.cols, sub_view_cv.type()); // pre-allocate

  for (auto _ : state)
  {
    cv::filter2D(sub_view_cv, img_dst_cv, -1, kernel_cv);
  }
}

#endif  // SELENE_IMG_OPENCV_HPP

void image_convolution_x_floating_point_kernel_rgb(benchmark::State& state) { image_convolution_x_floating_point_kernel<sln::PixelFormat::RGB>(state); }
void image_convolution_y_floating_point_kernel_rgb(benchmark::State& state) { image_convolution_y_floating_point_kernel<sln::PixelFormat::RGB>(state); }
void image_convolution_x_integer_kernel_rgb(benchmark::State& state) { image_convolution_x_integer_kernel<sln::PixelFormat::RGB>(state); }
void image_convolution_y_integer_kernel_rgb(benchmark::State& state) { image_convolution_y_integer_kernel<sln::PixelFormat::RGB>(state); }
#if defined(SELENE_WITH_OPENCV)
void image_convolution_x_opencv_rgb(benchmark::State& state) { image_convolution_x_opencv<sln::PixelFormat::RGB>(state); }
void image_convolution_y_opencv_rgb(benchmark::State& state) { image_convolution_y_opencv<sln::PixelFormat::RGB>(state); }
#endif  // SELENE_IMG_OPENCV_HPP

BENCHMARK(image_convolution_x_floating_point_kernel_rgb);
BENCHMARK(image_convolution_y_floating_point_kernel_rgb);
BENCHMARK(image_convolution_x_integer_kernel_rgb);
BENCHMARK(image_convolution_y_integer_kernel_rgb);
#if defined(SELENE_WITH_OPENCV)
BENCHMARK(image_convolution_x_opencv_rgb);
BENCHMARK(image_convolution_y_opencv_rgb);
#endif  // SELENE_IMG_OPENCV_HPP

void image_convolution_x_floating_point_kernel_y(benchmark::State& state) { image_convolution_x_floating_point_kernel<sln::PixelFormat::Y>(state); }
void image_convolution_y_floating_point_kernel_y(benchmark::State& state) { image_convolution_y_floating_point_kernel<sln::PixelFormat::Y>(state); }
void image_convolution_x_integer_kernel_y(benchmark::State& state) { image_convolution_x_integer_kernel<sln::PixelFormat::Y>(state); }
void image_convolution_y_integer_kernel_y(benchmark::State& state) { image_convolution_y_integer_kernel<sln::PixelFormat::Y>(state); }
#if defined(SELENE_WITH_OPENCV)
void image_convolution_x_opencv_y(benchmark::State& state) { image_convolution_x_opencv<sln::PixelFormat::Y>(state); }
void image_convolution_y_opencv_y(benchmark::State& state) { image_convolution_y_opencv<sln::PixelFormat::Y>(state); }
#endif  // SELENE_IMG_OPENCV_HPP

BENCHMARK(image_convolution_x_floating_point_kernel_y);
BENCHMARK(image_convolution_y_floating_point_kernel_y);
BENCHMARK(image_convolution_x_integer_kernel_y);
BENCHMARK(image_convolution_y_integer_kernel_y);
#if defined(SELENE_WITH_OPENCV)
BENCHMARK(image_convolution_x_opencv_y);
BENCHMARK(image_convolution_y_opencv_y);
#endif  // SELENE_IMG_OPENCV_HPP

BENCHMARK_MAIN();
