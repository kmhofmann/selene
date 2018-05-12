// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Round.hpp>

#include <random>

namespace {

template <typename T, typename IT>
void run_tests()
{
  std::mt19937 rng(800);
  std::uniform_real_distribution<T> dist(T(-1000000.0), T(1000000.0));

  for (std::size_t i = 0; i < 10000; ++i)
  {
    const auto val = dist(rng);
    REQUIRE(sln::constexpr_floor<IT>(val) == static_cast<IT>(std::floor(val)));
    REQUIRE(sln::constexpr_ceil<IT>(val) == static_cast<IT>(std::ceil(val)));
  }

  REQUIRE(sln::constexpr_floor<IT>(T(-2.0)) == -2);
  REQUIRE(sln::constexpr_floor<IT>(T(-1.75)) == -2);
  REQUIRE(sln::constexpr_floor<IT>(T(-1.5)) == -2);
  REQUIRE(sln::constexpr_floor<IT>(T(-1.25)) == -2);
  REQUIRE(sln::constexpr_floor<IT>(T(-1.0)) == -1);
  REQUIRE(sln::constexpr_floor<IT>(T(-0.75)) == -1);
  REQUIRE(sln::constexpr_floor<IT>(T(-0.5)) == -1);
  REQUIRE(sln::constexpr_floor<IT>(T(-0.25)) == -1);

  REQUIRE(sln::constexpr_floor<IT>(T(0.0)) == 0);

  REQUIRE(sln::constexpr_floor<IT>(T(0.25)) == 0);
  REQUIRE(sln::constexpr_floor<IT>(T(0.5)) == 0);
  REQUIRE(sln::constexpr_floor<IT>(T(0.75)) == 0);
  REQUIRE(sln::constexpr_floor<IT>(T(1.0)) == 1);
  REQUIRE(sln::constexpr_floor<IT>(T(1.25)) == 1);
  REQUIRE(sln::constexpr_floor<IT>(T(1.5)) == 1);
  REQUIRE(sln::constexpr_floor<IT>(T(1.75)) == 1);
  REQUIRE(sln::constexpr_floor<IT>(T(2.0)) == 2);

  // ---

  REQUIRE(sln::constexpr_ceil<IT>(T(-2.0)) == -2);
  REQUIRE(sln::constexpr_ceil<IT>(T(-1.75)) == -1);
  REQUIRE(sln::constexpr_ceil<IT>(T(-1.5)) == -1);
  REQUIRE(sln::constexpr_ceil<IT>(T(-1.25)) == -1);
  REQUIRE(sln::constexpr_ceil<IT>(T(-1.0)) == -1);
  REQUIRE(sln::constexpr_ceil<IT>(T(-0.75)) == 0);
  REQUIRE(sln::constexpr_ceil<IT>(T(-0.5)) == 0);
  REQUIRE(sln::constexpr_ceil<IT>(T(-0.25)) == 0);

  REQUIRE(sln::constexpr_ceil<IT>(T(0.0)) == 0);

  REQUIRE(sln::constexpr_ceil<IT>(T(0.25)) == 1);
  REQUIRE(sln::constexpr_ceil<IT>(T(0.5)) == 1);
  REQUIRE(sln::constexpr_ceil<IT>(T(0.75)) == 1);
  REQUIRE(sln::constexpr_ceil<IT>(T(1.0)) == 1);
  REQUIRE(sln::constexpr_ceil<IT>(T(1.25)) == 2);
  REQUIRE(sln::constexpr_ceil<IT>(T(1.5)) == 2);
  REQUIRE(sln::constexpr_ceil<IT>(T(1.75)) == 2);
  REQUIRE(sln::constexpr_ceil<IT>(T(2.0)) == 2);

  // ---

  REQUIRE(sln::round_half_up<IT>(T(-2.0)) == -2);
  REQUIRE(sln::round_half_up<IT>(T(-1.75)) == -2);
  REQUIRE(sln::round_half_up<IT>(T(-1.5)) == -1);
  REQUIRE(sln::round_half_up<IT>(T(-1.25)) == -1);
  REQUIRE(sln::round_half_up<IT>(T(-1.0)) == -1);
  REQUIRE(sln::round_half_up<IT>(T(-0.75)) == -1);
  REQUIRE(sln::round_half_up<IT>(T(-0.5)) == 0);
  REQUIRE(sln::round_half_up<IT>(T(-0.25)) == 0);

  REQUIRE(sln::round_half_up<IT>(T(0.0)) == 0);

  REQUIRE(sln::round_half_up<IT>(T(0.25)) == 0);
  REQUIRE(sln::round_half_up<IT>(T(0.5)) == 1);
  REQUIRE(sln::round_half_up<IT>(T(0.75)) == 1);
  REQUIRE(sln::round_half_up<IT>(T(1.0)) == 1);
  REQUIRE(sln::round_half_up<IT>(T(1.25)) == 1);
  REQUIRE(sln::round_half_up<IT>(T(1.5)) == 2);
  REQUIRE(sln::round_half_up<IT>(T(1.75)) == 2);
  REQUIRE(sln::round_half_up<IT>(T(2)) == 2);

  // ---

  REQUIRE(sln::round_half_down<IT>(T(-2)) == -2);
  REQUIRE(sln::round_half_down<IT>(T(-1.75)) == -2);
  REQUIRE(sln::round_half_down<IT>(T(-1.5)) == -2);
  REQUIRE(sln::round_half_down<IT>(T(-1.25)) == -1);
  REQUIRE(sln::round_half_down<IT>(T(-1.0)) == -1);
  REQUIRE(sln::round_half_down<IT>(T(-0.75)) == -1);
  REQUIRE(sln::round_half_down<IT>(T(-0.5)) == -1);
  REQUIRE(sln::round_half_down<IT>(T(-0.25)) == 0);

  REQUIRE(sln::round_half_down<IT>(T(0.0)) == 0);

  REQUIRE(sln::round_half_down<IT>(T(0.25)) == 0);
  REQUIRE(sln::round_half_down<IT>(T(0.5)) == 0);
  REQUIRE(sln::round_half_down<IT>(T(0.75)) == 1);
  REQUIRE(sln::round_half_down<IT>(T(1.0)) == 1);
  REQUIRE(sln::round_half_down<IT>(T(1.25)) == 1);
  REQUIRE(sln::round_half_down<IT>(T(1.5)) == 1);
  REQUIRE(sln::round_half_down<IT>(T(1.75)) == 2);
  REQUIRE(sln::round_half_down<IT>(T(2)) == 2);
}

}  // namespace _

TEST_CASE("Rounding", "[base]")
{
  run_tests<float, std::int32_t>();
  run_tests<double, std::int32_t>();
  run_tests<float, std::int64_t>();
  run_tests<double, std::int64_t>();
}
