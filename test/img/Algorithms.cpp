// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Algorithms.hpp>
#include <selene/img/Image.hpp>

#include <random>

using namespace sln::literals;

TEST_CASE("Image algorithms", "[img]")
{
  sln::Image_8u1 img(64_px, 64_px);
  img.fill(42);

  SECTION("Test for_each_pixel()")
  {
    sln::for_each_pixel(img, [](auto& px) { px = 67; });

    for (const auto& row : img)
    {
      for (const auto& px : row)
      {
        REQUIRE(px == 67);
      }
    }
  }

  SECTION("Test transform_pixels()")
  {
    const auto img2 = sln::transform_pixels<sln::Pixel_32u1>(img,
                                                             [](const auto& px) { return sln::Pixel_32u1{px / 2}; });

    for (const auto& row : img2)
    {
      for (const auto& px : row)
      {
        REQUIRE(px == 21);
      }
    }
  }
}