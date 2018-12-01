// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/TypedLayout.hpp>

using namespace sln::literals;

TEST_CASE("Typed layout", "[img]")
{
  SECTION("Default construction")
  {
    sln::TypedLayout ul0;

    REQUIRE(ul0.width == 0_px);
    REQUIRE(ul0.height == 0_px);
    REQUIRE(ul0.stride_bytes == sln::Stride{0});

    REQUIRE(ul0.nr_bytes_per_pixel<sln::Pixel_8u1>() == 1);
    REQUIRE(ul0.row_bytes<sln::Pixel_8u1>() == 0);
    REQUIRE(ul0.total_bytes<sln::Pixel_8u1>() == 0);
    REQUIRE(ul0.is_packed<sln::Pixel_8u1>());

    sln::TypedLayout ul1;

    REQUIRE(ul0 == ul1);
  }

  SECTION("Construction with arguments")
  {
    sln::TypedLayout ul0(36_px, 20_px);
    ul0.stride_bytes = sln::Stride{36 * 2};

    REQUIRE(ul0.width == 36_px);
    REQUIRE(ul0.height == 20_px);
    REQUIRE(ul0.stride_bytes == sln::Stride{72});

    REQUIRE(ul0.nr_bytes_per_pixel<sln::Pixel_16u1>() == 2);
    REQUIRE(ul0.row_bytes<sln::Pixel_16u1>() == 36 * 2);
    REQUIRE(ul0.total_bytes<sln::Pixel_16u1>() == 36 * 20 * 2);
    REQUIRE(ul0.is_packed<sln::Pixel_16u1>());

    sln::TypedLayout ul1(64_px, 24_px, sln::Stride{64 * 3 * 2 + 3});

    REQUIRE(ul1.width == 64_px);
    REQUIRE(ul1.height == 24_px);
    REQUIRE(ul1.stride_bytes == sln::Stride{64 * 6 + 3});

    REQUIRE(ul1.nr_bytes_per_pixel<sln::Pixel_16u3>() == 6);
    REQUIRE(ul1.row_bytes<sln::Pixel_16u3>() == 64 * 6);
    REQUIRE(ul1.total_bytes<sln::Pixel_16u3>() == (64 * 6 + 3) * 24);
    REQUIRE(!ul1.is_packed<sln::Pixel_16u3>());

    REQUIRE(ul0 != ul1);
  }
}

