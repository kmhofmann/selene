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

template <typename T, int N>
constexpr auto get_array_n_equal(T value)
{
  return sln::make_array_n_equal<T, N>(value);
}

template <typename T, int N>
void test_array_n_equal(const std::array<T, N>& arr, T value)
{
  for (auto i = 0; i < N; ++i)
  {
    REQUIRE(arr[i] == value);
  }
}

}  // namespace

TEST_CASE("Clamp", "[base]")
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

TEST_CASE("Make constexpr array", "[base")
{
  test_array_n_equal<unsigned char, 1>(get_array_n_equal<unsigned char, 1>(255), (unsigned char)(255));
  test_array_n_equal<int, 3>(get_array_n_equal<int, 3>(5), int(5));
  test_array_n_equal<float, 17>(get_array_n_equal<float, 17>(42.5), float(42.5));
  test_array_n_equal<double, 8>(get_array_n_equal<double, 8>(20.005), double(20.005));
}