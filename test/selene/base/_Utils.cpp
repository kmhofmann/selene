// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/base/Bitcount.hpp>
#include <selene/base/Promote.hpp>
#include <selene/base/Types.hpp>
#include <selene/base/Utils.hpp>

#include <algorithm>
#include <cstdint>
#include <random>
#include <type_traits>

#include <test/selene/Utils.hpp>

namespace {

template <typename T, int N>
void test_array_n_equal(const std::array<T, N>& arr, T value)
{
  for (auto i = std::size_t{0}; i < N; ++i)
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

  std::uint64_t operator()(std::size_t i)
  {
    return sequence.at(i);
  }
};

template <std::size_t N>
void test_array_from_function(std::uint32_t seed)
{
  Generator gen(seed, N);
  auto arr = sln::make_array_from_function<std::uint64_t, N>([&](std::size_t i) { return gen(i); });
  for (auto i = std::size_t{0}; i < N; ++i)
  {
    REQUIRE(arr[i] == gen.sequence[i]);
  }
}

}  // namespace

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

TEST_CASE("Next power of 2", "[base]")
{
  // Returns the position of the highest bit set;
  // from http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
  auto log_base_2 = [](auto x) {
    std::size_t r = 0;
    while (x >>= 1)
    {
      ++r;
    }
    return r;
  };

  std::mt19937 rng(42);
  std::uniform_int_distribution<std::uint32_t> dist32(std::numeric_limits<std::uint32_t>::min(),
                                                      std::numeric_limits<std::uint32_t>::max() / 2);
  std::uniform_int_distribution<std::uint64_t> dist64(std::numeric_limits<std::uint64_t>::min(),
                                                      std::numeric_limits<std::uint64_t>::max() / 2);

  for (std::size_t i = 0; i < 100000; ++i)
  {
    {
      const auto x = dist32(rng);
      const auto next_power = sln::next_power_of_two(x);
      REQUIRE(sln::bit_count(next_power) == 1);
      REQUIRE(log_base_2(next_power) == log_base_2(x) + (sln::bit_count(x) == 1 ? 0 : 1));
    }

    {
      const auto x = dist64(rng);
      const auto next_power = sln::next_power_of_two(x);
      REQUIRE(sln::bit_count(next_power) == 1);
      REQUIRE(log_base_2(next_power) == log_base_2(x) + (sln::bit_count(x) == 1 ? 0 : 1));
    }
  }

  // Explicitly test powers of 2
  REQUIRE(sln::next_power_of_two(0) == 0);
  std::size_t p2 = 1;
  for (std::size_t i = 0; i < 63; ++i, p2 <<= 1)
  {
    REQUIRE(sln::next_power_of_two(p2) == p2);
  }
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
  static constexpr std::array<float, 3> values = {{0.6f, 0.1f, 0.3f}};
};

TEST_CASE("Rounded linear combination", "[base]")
{
  // Check that we can evaluate at compile time
  constexpr auto r0 = sln::approximate_linear_combination<std::uint8_t, 3, TestCoefficients>(
      std::array<std::uint8_t, 3>{{50, 120, 255}});
  REQUIRE(r0 == 119);

  // Check that a linear combination of equal values yields the same value
  for (std::uint8_t i = 0; i < 255; ++i)
  {
    // This case needs a larger promoted type (uint32_t) than the default one (uint16_t), to not have errors from the
    // integer arithmetic from i=128 onwards.
    const auto r = sln::approximate_linear_combination<std::uint8_t, 3, TestCoefficients, std::uint32_t>(
        sln::make_array_n_equal<std::uint8_t, 3>(i));
    REQUIRE(r == i);
  }

  // Check that we can use floating point input values, although this is not an intended use case of the function
  const auto arr = sln::approximate_linear_combination<std::uint8_t, 3, TestCoefficients>(
      std::array<double, 3>{{50.0, 100.0, 200.0}});
  REQUIRE(arr == 100);
}
