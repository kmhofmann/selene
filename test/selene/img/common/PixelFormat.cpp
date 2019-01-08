// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/common/PixelFormat.hpp>

TEST_CASE("Pixel format alpha value conversion query", "[img]")
{
  REQUIRE(sln::conversion_requires_alpha_value(sln::PixelFormat::Y, sln::PixelFormat::YA));
  REQUIRE(sln::conversion_requires_alpha_value(sln::PixelFormat::RGB, sln::PixelFormat::ABGR));
  REQUIRE(sln::conversion_requires_alpha_value(sln::PixelFormat::BGR, sln::PixelFormat::ABGR));
  // ...

  REQUIRE(!sln::conversion_requires_alpha_value(sln::PixelFormat::YA, sln::PixelFormat::Y));
  REQUIRE(!sln::conversion_requires_alpha_value(sln::PixelFormat::ARGB, sln::PixelFormat::BGR));
  REQUIRE(!sln::conversion_requires_alpha_value(sln::PixelFormat::ABGR, sln::PixelFormat::BGR));
  // ...
}
