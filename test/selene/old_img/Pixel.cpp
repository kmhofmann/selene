// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>

#include <selene/old_img/Pixel.hpp>
#include <selene/old_img/PixelTraits.hpp>

using namespace sln::literals;

TEST_CASE("Pixel operations", "[old_img]")
{
  sln::Pixel<std::uint8_t, 3> px0(42, 13, 8);
  REQUIRE(px0[0] == 42);
  REQUIRE(px0[1] == 13);
  REQUIRE(px0[2] == 8);

  SECTION("Initialization with elements")
  {
    px0 = sln::Pixel<std::uint8_t, 3>(11, 22, 33);
    REQUIRE(px0[0] == 11);
    REQUIRE(px0[1] == 22);
    REQUIRE(px0[2] == 33);
  }

  SECTION("Element assignment via pointer")
  {
    auto px0_ptr = px0.data();
    REQUIRE(*(px0_ptr + 0) == 42);
    REQUIRE(*(px0_ptr + 1) == 13);
    REQUIRE(*(px0_ptr + 2) == 8);
    px0_ptr[1] = 66;
    REQUIRE(*(px0_ptr + 1) == 66);
  }


  SECTION("Assignment via array")
  {
    std::array<std::int32_t, 6> arr = {{123, 6001, -32, 65535, 0, -40000}};
    const sln::Pixel<std::int32_t, 6> px1(arr);
    REQUIRE(px1[0] == 123);
    REQUIRE(px1[1] == 6001);
    REQUIRE(px1[2] == -32);
    REQUIRE(px1[3] == 65535);
    REQUIRE(px1[4] == 0);
    REQUIRE(px1[5] == -40000);
  }

  SECTION("Floating point pixel")
  {
    const sln::Pixel<sln::float64_t, 2> px2(1.0, 2.0);
    REQUIRE(px2[0] == 1.0);
    REQUIRE(px2[1] == 2.0);
  }

  SECTION("Implicit conversion")
  {
    sln::Pixel<std::uint8_t, 1> px3(192);
    REQUIRE(px3 == 192);  // test implicit conversion
  }

  SECTION("Copy construction")
  {
    sln::Pixel<std::int16_t, 3> px4 = px0;
    REQUIRE(px4[0] == 42);
    REQUIRE(px4[1] == 13);
    REQUIRE(px4[2] == 8);
    px4[2] = 127;

    sln::Pixel<std::int8_t, 3> px5 = px4;
    REQUIRE(px5[0] == 42);
    REQUIRE(px5[1] == 13);
    REQUIRE(px5[2] == 127);
  }

// We can only use the below pixel arithmetic in a constexpr context with C++ 17, since std::array<>::operator[] is not
// declared constexpr before C++17.
#if __cplusplus >= 201703L
#define SELENE_CONSTEXPR constexpr
#else
#define SELENE_CONSTEXPR const
#endif

  SECTION("Pixel arithmetic 1")
  {
    SELENE_CONSTEXPR auto px1 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) + std::uint8_t{40};
    REQUIRE(px1 == sln::Pixel<std::uint8_t, 3>(50, 60, 70));

    SELENE_CONSTEXPR auto px2 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) + sln::Pixel<std::uint8_t, 3>(5, 6, 7);
    REQUIRE(px2 == sln::Pixel<std::uint8_t, 3>(15, 26, 37));

    SELENE_CONSTEXPR auto px3 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) - std::uint8_t{10};
    REQUIRE(px3 == sln::Pixel<std::uint8_t, 3>(0, 10, 20));

    SELENE_CONSTEXPR auto px4 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) - sln::Pixel<std::uint8_t, 3>(1, 2, 3);
    REQUIRE(px4 == sln::Pixel<std::uint8_t, 3>(9, 18, 27));

    SELENE_CONSTEXPR auto px5 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) * std::uint8_t{3};
    REQUIRE(px5 == sln::Pixel<std::uint8_t, 3>(30, 60, 90));

    SELENE_CONSTEXPR auto px6 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) * sln::Pixel<std::uint8_t, 3>(5, 3, 1);
    REQUIRE(px6 == sln::Pixel<std::uint8_t, 3>(50, 60, 30));

    SELENE_CONSTEXPR auto px7 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) / std::uint8_t{2};
    REQUIRE(px7 == sln::Pixel<std::uint8_t, 3>(5, 10, 15));

    SELENE_CONSTEXPR auto px8 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) / sln::Pixel<std::uint8_t, 3>(2, 4, 6);
    REQUIRE(px8 == sln::Pixel<std::uint8_t, 3>(5, 5, 5));
  }

  SECTION("Pixel arithmetic 2")
  {
    SELENE_CONSTEXPR auto px1 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) + std::uint32_t{40};
    REQUIRE(std::is_same<std::decay_t<decltype(px1)>, sln::Pixel<std::uint32_t, 3>>::value);
    REQUIRE(px1 == sln::Pixel<std::uint32_t, 3>(50, 60, 70));

    SELENE_CONSTEXPR auto px2 = std::uint32_t{40} + sln::Pixel<std::uint8_t, 3>(10, 20, 30);
    REQUIRE(std::is_same<std::decay_t<decltype(px2)>, sln::Pixel<std::uint32_t, 3>>::value);
    REQUIRE(px2 == sln::Pixel<std::uint32_t, 3>(50, 60, 70));

    SELENE_CONSTEXPR auto px3 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) * std::uint32_t{5};
    REQUIRE(std::is_same<std::decay_t<decltype(px3)>, sln::Pixel<std::uint32_t, 3>>::value);
    REQUIRE(px3 == sln::Pixel<std::uint32_t, 3>(50, 100, 150));

    SELENE_CONSTEXPR auto px4 = std::uint32_t{5} * sln::Pixel<std::uint8_t, 3>(10, 20, 30);
    REQUIRE(std::is_same<std::decay_t<decltype(px4)>, sln::Pixel<std::uint32_t, 3>>::value);
    REQUIRE(px4 == sln::Pixel<std::uint32_t, 3>(50, 100, 150));

    SELENE_CONSTEXPR auto px5 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) + sln::float32_t{40.0f};
    REQUIRE(std::is_same<std::decay_t<decltype(px5)>, sln::Pixel<sln::float32_t, 3>>::value);
    REQUIRE(px5 == sln::Pixel<sln::float32_t, 3>(50.0f, 60.0f, 70.0f));

    SELENE_CONSTEXPR auto px6 = sln::Pixel<std::uint8_t, 3>(10, 20, 30) + sln::float32_t{40.0f};
    REQUIRE(std::is_same<std::decay_t<decltype(px6)>, sln::Pixel<sln::float32_t, 3>>::value);
    REQUIRE(px6 == sln::Pixel<sln::float32_t, 3>(50.0f, 60.0f, 70.0f));

    SELENE_CONSTEXPR auto px7 = -sln::Pixel<std::int32_t, 3>(10, 20, 30);
    REQUIRE(px7 == sln::Pixel<std::int32_t, 3>(-10, -20, -30));
  }
}
