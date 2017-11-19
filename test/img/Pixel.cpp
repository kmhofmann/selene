// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Pixel.hpp>

using namespace selene;
using namespace selene::img;

TEST_CASE("Pixel construction", "[img]")
{
  Pixel<std::uint8_t, 3> px0(42, 13, 8);
  REQUIRE(px0[0] == 42);
  REQUIRE(px0[1] == 13);
  REQUIRE(px0[2] == 8);

  px0 = Pixel<std::uint8_t, 3>(11, 22, 33);
  REQUIRE(px0[0] == 11);
  REQUIRE(px0[1] == 22);
  REQUIRE(px0[2] == 33);

  auto px0_ptr = px0.data();
  REQUIRE(*(px0_ptr + 0) == 11);
  REQUIRE(*(px0_ptr + 1) == 22);
  REQUIRE(*(px0_ptr + 2) == 33);
  px0_ptr[1] = 66;
  REQUIRE(*(px0_ptr + 1) == 66);

  std::array<std::int32_t, 6> arr = {{ 123, 6001, -32, 65535, 0, -40000 }};
  const Pixel<std::int32_t, 6> px1(arr);
  REQUIRE(px1[0] == 123);
  REQUIRE(px1[1] == 6001);
  REQUIRE(px1[2] == -32);
  REQUIRE(px1[3] == 65535);
  REQUIRE(px1[4] == 0);
  REQUIRE(px1[5] == -40000);

  const Pixel<float64_t, 2> px2(1.0, 2.0);
  REQUIRE(px2[0] == 1.0);
  REQUIRE(px2[1] == 2.0);

  Pixel<std::uint8_t, 1> px3(192);
  REQUIRE(px3 == 192); // test implicit conversion
}
