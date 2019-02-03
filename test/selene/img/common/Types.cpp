// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/common/Types.hpp>

TEST_CASE("Pixel index/length conversion", "[img]")
{
  REQUIRE(sln::to_pixel_index(10) == sln::PixelIndex{10});
  REQUIRE(sln::to_pixel_index(-10) == sln::PixelIndex{-10});

  REQUIRE(sln::to_pixel_length(10) == sln::PixelLength{10});
  REQUIRE(sln::to_pixel_length(-10) == sln::PixelLength{-10});

  using namespace sln::literals;

  REQUIRE(10_idx == sln::PixelIndex{10});
  REQUIRE(-10_idx == sln::PixelIndex{-10});

  REQUIRE(10_px == sln::PixelLength{10});
  REQUIRE(-10_px == sln::PixelLength{-10});
}
