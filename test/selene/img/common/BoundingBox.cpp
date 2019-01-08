// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/common/BoundingBox.hpp>

using namespace sln::literals;

TEST_CASE("Bounding box construction", "[img]")
{
  SECTION("Default construction")
  {
    sln::BoundingBox b;
    REQUIRE(b.x0() == 0_idx);
    REQUIRE(b.y0() == 0_idx);
    REQUIRE(b.width() == 0_px);
    REQUIRE(b.height() == 0_px);
    REQUIRE(b.x1() == 0_idx);
    REQUIRE(b.y1() == 0_idx);
    REQUIRE(b.empty());
  }

  SECTION("Construction with width/height")
  {
    sln::BoundingBox b(10_idx, 20_idx, 30_px, 40_px);
    REQUIRE(b.x0() == 10_idx);
    REQUIRE(b.y0() == 20_idx);
    REQUIRE(b.width() == 30_px);
    REQUIRE(b.height() == 40_px);
    REQUIRE(b.x1() == 40_idx);
    REQUIRE(b.y1() == 60_idx);
    REQUIRE(!b.empty());
  }

  SECTION("Construction with x1/y1")
  {
    sln::BoundingBox b(10_idx, 20_idx, 30_idx, 40_idx);
    REQUIRE(b.x0() == 10_idx);
    REQUIRE(b.y0() == 20_idx);
    REQUIRE(b.width() == 20_px);
    REQUIRE(b.height() == 20_px);
    REQUIRE(b.x1() == 30_idx);
    REQUIRE(b.y1() == 40_idx);
    REQUIRE(!b.empty());
  }
}

TEST_CASE("Bounding box sanitization")
{
  sln::BoundingBox b0(0_idx, 0_idx, 50_px, 80_px);
  b0.sanitize(30_px, 40_px);
  REQUIRE(b0.x0() == 0_idx);
  REQUIRE(b0.y0() == 0_idx);
  REQUIRE(b0.width() == 30_px);
  REQUIRE(b0.height() == 40_px);
  REQUIRE(b0.x1() == 30_idx);
  REQUIRE(b0.y1() == 40_idx);
  REQUIRE(!b0.empty());

  sln::BoundingBox b1(-10_idx, -10_idx, 30_px, 20_px);
  b1.sanitize(30_px, 40_px);
  REQUIRE(b1.x0() == 0_idx);
  REQUIRE(b1.y0() == 0_idx);
  REQUIRE(b1.width() == 30_px);
  REQUIRE(b1.height() == 20_px);
  REQUIRE(b1.x1() == 30_idx);
  REQUIRE(b1.y1() == 20_idx);
  REQUIRE(!b1.empty());
}
