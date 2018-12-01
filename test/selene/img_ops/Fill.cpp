// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img_ops/Fill.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

using namespace sln::literals;

TEST_CASE("Image filling", "[img]")
{
  sln::Image_8u1 img({32_px, 32_px});
  sln::fill(img, 42);

  for (const auto& row : img)
  {
    for (const auto& px : row)
    {
      REQUIRE(px == 42);
    }
  }
}
