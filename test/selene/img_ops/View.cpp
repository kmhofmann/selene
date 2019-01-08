// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/View.hpp>

#include <test/selene/img/typed/_Utils.hpp>

using namespace sln::literals;

TEST_CASE("Image view creation", "[img]")
{
  auto img0 = sln_test::make_3x3_test_image_8u1();
  auto img0_view = sln::view(img0);

  REQUIRE(img0(1_idx, 1_idx) == 50);
  img0_view(1_idx, 1_idx) = 42;
  REQUIRE(img0(1_idx, 1_idx) == 42);

  const auto img1 = sln_test::make_3x3_test_image_8u1();
  auto img1_view = sln::view(img1);

  REQUIRE(img1(1_idx, 1_idx) == 50);
  REQUIRE(img1_view(1_idx, 1_idx) == 50);
}