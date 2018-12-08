// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/dynamic/UntypedLayout.hpp>

using namespace sln::literals;

TEST_CASE("Untyped layout", "[img]")
{
  SECTION("Default construction")
  {
    sln::UntypedLayout ul0;

    REQUIRE(ul0.width == 0_px);
    REQUIRE(ul0.height == 0_px);
    REQUIRE(ul0.nr_channels == 0);
    REQUIRE(ul0.nr_bytes_per_channel == 0);
    REQUIRE(ul0.stride_bytes == sln::Stride{0});

    REQUIRE(ul0.nr_bytes_per_pixel() == 0);
    REQUIRE(ul0.row_bytes() == 0);
    REQUIRE(ul0.total_bytes() == 0);
    REQUIRE(ul0.is_packed());

    sln::UntypedLayout ul1;

    REQUIRE(ul0 == ul1);
  }

  SECTION("Construction with arguments")
  {
    sln::UntypedLayout ul0(36_px, 20_px, 2, 1);

    REQUIRE(ul0.width == 36_px);
    REQUIRE(ul0.height == 20_px);
    REQUIRE(ul0.nr_channels == 2);
    REQUIRE(ul0.nr_bytes_per_channel == 1);
    REQUIRE(ul0.stride_bytes == sln::Stride{36 * 2});

    REQUIRE(ul0.nr_bytes_per_pixel() == 2);
    REQUIRE(ul0.row_bytes() == 36 * 2);
    REQUIRE(ul0.total_bytes() == 36 * 20 * 2);
    REQUIRE(ul0.is_packed());

    sln::UntypedLayout ul1(64_px, 24_px, 3, 2, sln::Stride{64 * 3 * 2 + 3});

    REQUIRE(ul1.width == 64_px);
    REQUIRE(ul1.height == 24_px);
    REQUIRE(ul1.nr_channels == 3);
    REQUIRE(ul1.nr_bytes_per_channel == 2);
    REQUIRE(ul1.stride_bytes == sln::Stride{64 * 6 + 3});

    REQUIRE(ul1.nr_bytes_per_pixel() == 6);
    REQUIRE(ul1.row_bytes() == 64 * 6);
    REQUIRE(ul1.total_bytes() == (64 * 6 + 3) * 24);
    REQUIRE(!ul1.is_packed());

    REQUIRE(ul0 != ul1);
  }
}
