// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/Utilities.hpp>

using namespace sln::literals;

TEST_CASE("Counting number of image channels", "[img]")
{
  sln::Image_8u1 img_1c;
  sln::Image_8u2 img_2c;
  sln::Image_8u3 img_3c;
  sln::Image_8u4 img_4c;
  sln::Image<sln::Pixel<std::uint8_t, 20>> img_20c;

  REQUIRE(sln::count_nr_channels(img_1c) == 1);
  REQUIRE(sln::count_nr_channels(img_2c) == 2);
  REQUIRE(sln::count_nr_channels(img_3c) == 3);
  REQUIRE(sln::count_nr_channels(img_4c) == 4);
  REQUIRE(sln::count_nr_channels(img_20c) == 20);

  REQUIRE(sln::count_nr_channels(img_1c, img_1c) == 2);
  REQUIRE(sln::count_nr_channels(img_1c, img_1c, img_1c) == 3);
  REQUIRE(sln::count_nr_channels(img_1c, img_1c, img_1c, img_1c) == 4);
  REQUIRE(sln::count_nr_channels(img_1c, img_1c, img_1c, img_1c, img_1c) == 5);

  REQUIRE(sln::count_nr_channels(img_1c, img_2c) == 3);
  REQUIRE(sln::count_nr_channels(img_3c, img_4c) == 7);
  REQUIRE(sln::count_nr_channels(img_20c, img_4c) == 24);
}
