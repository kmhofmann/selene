// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/Assert.hpp>
#include <selene/base/Kernel.hpp>
#include <selene/base/io/FileReader.hpp>

#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/access/BorderAccessors.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Convolution.hpp>
#include <selene/img_ops/View.hpp>

#include <test/selene/Utils.hpp>

#include <boost/filesystem.hpp>

#include <benchmark/benchmark.h>

#include <tuple>

using namespace sln::literals;
namespace fs = boost::filesystem;

namespace {

sln::ImageRGB_8u read_image(const std::string& filename)
{
  const auto full_path = sln_test::full_data_path(filename.c_str());
  auto dyn_img = sln::read_image(sln::FileReader(full_path.string()));
  SELENE_FORCED_ASSERT(dyn_img.is_valid());
  return sln::to_image<sln::PixelRGB_8u>(std::move(dyn_img));
}

auto get_stuff()
{
  auto img = read_image("stickers.png");
  auto sub_view = sln::view(img, {10_idx, 10_idx, 20_px, 20_px});  // 20x20 pixel view
  auto kernel = sln::gaussian_kernel<7>(1.0);
  return std::tuple{std::move(img), sub_view, kernel};
}

}  // namespace _

void image_convolution_x_floating_point_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff();
  sln::ImageRGB_8u img_dst;

  for (auto _ : state)
  {
    sln::convolution_x<sln::BorderAccessMode::Unchecked>(sub_view, img_dst, kernel);
  }
}

void image_convolution_y_floating_point_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff();
  sln::ImageRGB_8u img_dst;

  for (auto _ : state)
  {
    sln::convolution_y<sln::BorderAccessMode::Unchecked>(sub_view, img_dst, kernel);
  }
}

void image_convolution_x_integer_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff();
  constexpr auto shift = 16u;
  const auto integer_kernel = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel);
  sln::ImageRGB_8u img_dst;

  for (auto _ : state)
  {
    sln::convolution_x<sln::BorderAccessMode::Unchecked, shift>(sub_view, img_dst, integer_kernel);
  }
}

void image_convolution_y_integer_kernel(benchmark::State& state)
{
  auto [img, sub_view, kernel] = get_stuff();
  constexpr auto shift = 16u;
  const auto integer_kernel = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel);
  sln::ImageRGB_8u img_dst;

  for (auto _ : state)
  {
    sln::convolution_y<sln::BorderAccessMode::Unchecked, shift>(sub_view, img_dst, integer_kernel);
  }
}

BENCHMARK(image_convolution_x_floating_point_kernel);
BENCHMARK(image_convolution_y_floating_point_kernel);
BENCHMARK(image_convolution_x_integer_kernel);
BENCHMARK(image_convolution_y_integer_kernel);

BENCHMARK_MAIN();
