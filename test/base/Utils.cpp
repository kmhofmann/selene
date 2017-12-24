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
void test_array_n_equal(const std::array<T, N>& arr, T value)
{
  for (auto i = 0; i < N; ++i)
  {
    REQUIRE(arr[i] == value);
  }
}

struct Generator
{
  std::vector<std::uint64_t> sequence;

  explicit Generator(std::uint32_t seed, std::size_t nr_values)
  {
    std::mt19937 rng{seed};
    std::uniform_int_distribution<std::uint64_t> dist{0ul, 1000000ul};
    sequence.resize(nr_values);
    std::generate(std::begin(sequence), std::end(sequence), [&]() { return dist(rng); });
  }

  float operator()(std::size_t i)
  {
    return sequence.at(i);
  }
};

template <std::size_t N>
void test_array_from_function(std::uint32_t seed)
{
  Generator gen(seed, N);
  auto arr = sln::make_array_from_function<std::uint64_t, N>([&](std::size_t i) { return gen(i); });
  for (std::size_t i = 0; i < N; ++i)
  {
    REQUIRE(arr[i] == gen.sequence[i]);
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

TEST_CASE("Power", "[base]")
{
  constexpr auto pow_2_8 = sln::power(2u, 8u);
  REQUIRE(pow_2_8 == 256u);

  constexpr auto pow_3_5 = sln::power(3, 5u);
  REQUIRE(pow_3_5 == 243);

  constexpr auto pow_2d5_10 = sln::power(2.5, 10u);
  REQUIRE(pow_2d5_10 == Approx(9536.7431640625));

  constexpr auto pow_10_0 = sln::power(10, 0u);
  REQUIRE(pow_10_0 == 1);
}

TEST_CASE("Make array functions", "[base]")
{
  test_array_n_equal<unsigned char, 1>(sln::make_array_n_equal<unsigned char, 1>(255), (unsigned char)(255));
  test_array_n_equal<int, 3>(sln::make_array_n_equal<int, 3>(5), int(5));
  test_array_n_equal<float, 17>(sln::make_array_n_equal<float, 17>(42.5), float(42.5));
  test_array_n_equal<double, 8>(sln::make_array_n_equal<double, 8>(20.005), double(20.005));

  test_array_from_function<10>(42u);
  test_array_from_function<32>(12u);
  test_array_from_function<100>(78u);
}

struct TestCoefficients
{
  static constexpr std::array<float, 3> values = {0.6, 0.1, 0.3};
};

TEST_CASE("Rounded linear combination", "[base]")
{
  // Check that we can evaluate at compile time
  constexpr auto r0 = sln::rounded_linear_combination<std::uint8_t, 3, TestCoefficients>(
      std::array<std::uint8_t, 3>{{50, 120, 255}});
  REQUIRE(r0 == 119);

  // Check that a linear combination of equal values yields the same value
  for (std::uint8_t i = 0; i < 255; ++i)
  {
    // This case needs a larger promoted type (uint32_t) than the default one (uint16_t), to not have errors from the
    // integer arithmetic from i=128 onwards.
    const auto r = sln::rounded_linear_combination<std::uint8_t, 3, TestCoefficients, std::uint32_t>(
        sln::make_array_n_equal<std::uint8_t, 3>(i));
    REQUIRE(r == i);
  }

  // Check that we can use floating point input values
  const auto arr = sln::rounded_linear_combination<std::uint8_t, 3, TestCoefficients>(
      std::array<double, 3>{{50.0, 100.0, 200.0}});
  REQUIRE(arr == 100);
}