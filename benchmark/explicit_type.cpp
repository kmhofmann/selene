// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/_impl/ExplicitType.hpp>
#include <selene/base/Types.hpp>
#include <selene/img/common/Types.hpp>

#include <benchmark/benchmark.h>

//#define SELENE_BENCHMARK_DO_NOT_OPTIMIZE(x) benchmark::DoNotOptimize(x)
#define SELENE_BENCHMARK_DO_NOT_OPTIMIZE(x) (x)  // This should not result in any runtime computations at any
                                                 // reasonable optimization level.

void explicit_types(benchmark::State& state)
{
  using namespace sln::literals;

  for (auto _ : state)
  {
    auto sum2 = 0_b;

    for (auto idx2 = 0_b; idx2 < 100_b; ++idx2)
    {
      auto sum = 0_px;

      for (auto idx = 0_idx; idx < 100_idx; ++idx)
      {
        if (idx % 2 == 0)
        {
          SELENE_BENCHMARK_DO_NOT_OPTIMIZE(sum += sln::PixelLength{idx});
        }
      }

      SELENE_BENCHMARK_DO_NOT_OPTIMIZE(sum2 += sln::Bytes{sum});
    }
  }
}

void raw_types(benchmark::State& state)
{
  using BytesType = sln::Bytes::value_type;
  using PixelIndexType = sln::PixelIndex::value_type;
  using PixelLengthType = sln::PixelLength::value_type;

  for (auto _ : state)
  {
    auto sum2 = BytesType{0};

    for (auto idx2 = BytesType{0}; idx2 < BytesType{100}; ++idx2)
    {
      auto sum = PixelLengthType{0};

      for (auto idx = PixelIndexType{0}; idx < PixelIndexType{100}; ++idx)
      {
        if (idx % 2 == 0)
        {
          SELENE_BENCHMARK_DO_NOT_OPTIMIZE(sum += idx);
        }
      }

      SELENE_BENCHMARK_DO_NOT_OPTIMIZE(sum2 += sum);
    }
  }
}

BENCHMARK(explicit_types);
BENCHMARK(raw_types);

BENCHMARK_MAIN();
