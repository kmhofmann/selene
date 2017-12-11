// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>
#include <selene/base/Utils.hpp>

#include <algorithm>
#include <cstdint>
#include <random>
#include <type_traits>

namespace {

template <typename T>
void test_clamp(const T& value, const T& min, const T& max)
{
  const auto result = sln::clamp(value, min, max);

  REQUIRE(!(result < min));
  REQUIRE(!(result > max));

  if (value > min && value < max)
  {
    REQUIRE(result == value);
  }
  else if (value <= min)
  {
    REQUIRE(result == min);
  }
  else if (value >= max)
  {
    REQUIRE(result == max);
  }
}

template <typename T, typename RNG>
void test_clamps(RNG& rng, int nr_trials)
{
  constexpr auto is_int = std::is_integral<T>::value;
  constexpr auto is_fp = std::is_floating_point<T>::value;
  using DieType = std::conditional_t<is_int, std::uniform_int_distribution<T>,
                                     std::conditional_t<is_fp, std::uniform_real_distribution<T>, void>>;
  DieType die;

  for (auto i = 0; i < nr_trials; ++i)
  {
    const auto value0 = die(rng);
    const auto value1 = die(rng);
    const auto value = (i % 5 == 0) ? value0 : die(rng);
    test_clamp(value, std::min(value0, value1), std::max(value0, value1));
  }
}

}  // namespace

TEST_CASE("Clamp")
{
  std::mt19937 rng(42ul);

  constexpr auto nr_trials = 1000;
  test_clamps<std::uint8_t>(rng, nr_trials);
  test_clamps<std::int8_t>(rng, nr_trials);
  test_clamps<std::uint16_t>(rng, nr_trials);
  test_clamps<std::int16_t>(rng, nr_trials);
  test_clamps<std::uint32_t>(rng, nr_trials);
  test_clamps<std::int32_t>(rng, nr_trials);
  test_clamps<std::uint64_t>(rng, nr_trials);
  test_clamps<std::int64_t>(rng, nr_trials);
  test_clamps<sln::float32_t>(rng, nr_trials);
  test_clamps<sln::float64_t>(rng, nr_trials);
}
