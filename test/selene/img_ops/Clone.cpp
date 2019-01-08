// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Clone.hpp>

#include <test/selene/img/typed/_Utils.hpp>

TEST_CASE("Image cloning", "[img]")
{
  const auto img_x = sln_test::make_3x3_test_image_8u1();
  const auto img_x_2 = sln::clone(img_x);
  REQUIRE(img_x_2.width() == img_x.width());
  REQUIRE(img_x_2.height() == img_x.height());
  REQUIRE(img_x_2 == img_x);

  const auto img_xxx = sln_test::make_3x3_test_image_8u3();
  const auto img_xxx_2 = sln::clone(img_xxx);
  REQUIRE(img_xxx_2.width() == img_xxx.width());
  REQUIRE(img_xxx_2.height() == img_xxx.height());
  REQUIRE(img_xxx_2 == img_xxx);
}
