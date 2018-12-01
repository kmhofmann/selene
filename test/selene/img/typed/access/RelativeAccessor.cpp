// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/access/RelativeAccessor.hpp>

#include <test/selene/img/typed/_Utils.hpp>

using namespace sln::literals;

template <typename ImageType>
void test_relative_accessor(ImageType& img)
{
  auto ra = relative_accessor(img, 1_idx, 1_idx);

  REQUIRE(ra.anchor_x() == 1_idx);
  REQUIRE(ra.anchor_y() == 1_idx);

  REQUIRE(ra.get(-1_idx, -1_idx) == img(0_idx, 0_idx));
  REQUIRE(ra.get( 0_idx, -1_idx) == img(1_idx, 0_idx));
  REQUIRE(ra.get( 1_idx, -1_idx) == img(2_idx, 0_idx));

  REQUIRE(ra.get(-1_idx,  0_idx) == img(0_idx, 1_idx));
  REQUIRE(ra.get( 0_idx,  0_idx) == img(1_idx, 1_idx));
  REQUIRE(ra.get( 1_idx,  0_idx) == img(2_idx, 1_idx));

  REQUIRE(ra.get(-1_idx,  1_idx) == img(0_idx, 2_idx));
  REQUIRE(ra.get( 0_idx,  1_idx) == img(1_idx, 2_idx));
  REQUIRE(ra.get( 1_idx,  1_idx) == img(2_idx, 2_idx));
}

template <typename ImageType>
void test_relative_accessor_modifying(ImageType& img)
{
  auto ra = relative_accessor(img, 1_idx, 1_idx);

  ra.get(0_idx, 0_idx) = sln::Pixel<std::uint8_t, 1>{42};
  REQUIRE(img(1_idx, 1_idx) == sln::Pixel<std::uint8_t, 1>{42});
}

TEST_CASE("Relative accessor", "[img]")
{
  auto img = sln_test::make_3x3_test_image_8u1();
  auto view_m = sln::ImageView<sln::Pixel<std::uint8_t, 1>, sln::ImageModifiability::Mutable>(img.byte_ptr(), img.layout());
  const auto view_c = sln::ImageView<sln::Pixel<std::uint8_t, 1>, sln::ImageModifiability::Constant>(img.byte_ptr(), img.layout());

  SECTION("Not modifying")
  {
    test_relative_accessor(img);
    test_relative_accessor(view_m);
    test_relative_accessor(view_c);
  }

  SECTION("Modifying/image")
  {
    test_relative_accessor_modifying(img);
  }

  SECTION("Modifying/view")
  {
    test_relative_accessor_modifying(view_m);
  }
}
