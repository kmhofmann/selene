// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/Image.hpp>
#include <selene/img/ImageAccess.hpp>
#include <selene/img/RelativeAccessor.hpp>

#include <benchmark/benchmark.h>

using namespace sln::literals;

void image_access_member_fn(benchmark::State& state)
{
  sln::Image_8u3 img(100_px, 10_px);

  for (auto _ : state)
  {
    for (auto x = 0_px; x < img.width(); ++x)
    {
      benchmark::DoNotOptimize(
          img(x, 0_px)
      );
    }
  }
}

void image_access_get_fn(benchmark::State& state)
{
  sln::Image_8u3 img(100_px, 10_px);

  for (auto _ : state)
  {
    for (auto x = 0_px; x < img.width(); ++x)
    {
      benchmark::DoNotOptimize(
          get(img, x, 0_px)
      );
    }
  }
}

void image_access_relative(benchmark::State& state)
{
  sln::Image_8u3 img(100_px, 10_px);

  auto rel = sln::relative_accessor(img, 50_px, 5_px);

  for (auto _ : state)
  {
    for (sln::SignedPixelIndex dx = -50; dx < sln::SignedPixelIndex(img.width() - 50); ++dx)
    {
      benchmark::DoNotOptimize(
          rel.get(dx, 0)
      );
    }
  }
}

BENCHMARK(image_access_member_fn);
BENCHMARK(image_access_get_fn);
BENCHMARK(image_access_relative);

BENCHMARK_MAIN();