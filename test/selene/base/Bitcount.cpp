// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/base/Bitcount.hpp>

#include <random>
#include <type_traits>

#include <test/selene/Utils.hpp>


namespace {

template <typename T>
std::size_t simple_bitcount(T x)
{
  static_assert(std::is_unsigned<T>::value, "type not unsigned");
  std::size_t count = 0;

  while (x > 0)
  {
    count += x & T{1};
    x = T(x >> 1);
  };

  return count;
}

template <typename T>
void test_bitcount(std::mt19937& rng)
{
  auto dist = sln_test::uniform_distribution<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());

  for (std::size_t i = 0; i < 100000; ++i)
  {
    const auto x = static_cast<T>(dist(rng));
    const auto bc = sln::bit_count(x);
    const auto sbc = simple_bitcount(x);
    REQUIRE(bc == sbc);
  }
}

}  // namespace

TEST_CASE("Bitcount", "[base]")
{
  std::mt19937 rng(500);
  test_bitcount<std::uint8_t>(rng);
  test_bitcount<std::uint16_t>(rng);
  test_bitcount<std::uint32_t>(rng);
  test_bitcount<std::uint64_t>(rng);
}
