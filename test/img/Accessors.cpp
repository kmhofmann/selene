// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Accessors.hpp>
#include <selene/img/Image.hpp>

#include "_TestImages.hpp"

using namespace sln::literals;


TEST_CASE("Image accessors", "[img]")
{
  const auto img = sln_test::make_3x3_test_image_8u1();

  // within bounds

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Unchecked>::access(img, 0, 0) == 10);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, 0, 0) == 10);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, 0, 0) == 10);

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Unchecked>::access(img, 2, 1) == 60);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, 2, 1) == 60);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, 2, 1) == 60);

  // out of bounds

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, -1, 0) == 0);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, -1, 0) == 10);

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, 3, 0) == 0);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, 3, 0) == 30);

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, -1, 1) == 0);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, -1, 1) == 40);

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, -2, 1) == 0);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, -2, 1) == 40);

  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::ZeroPadding>::access(img, 1, 3) == 0);
  REQUIRE(sln::ImageAccessor<sln::BorderAccessMode::Replicated>::access(img, 1, 3) == 80);
}