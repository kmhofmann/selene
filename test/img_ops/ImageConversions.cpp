// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img_ops/ImageConversions.hpp>

#include <test/img/_TestImages.hpp>

using namespace sln::literals;

TEST_CASE("Image conversions", "[img]")
{
  const auto img_x = sln_test::make_3x3_test_image_8u1();
  const auto img_xxx = sln_test::make_3x3_test_image_8u3();

  // Just covering a few select conversions for now...

  SECTION("Convert Y to RGBA")
  {
    const auto img_rgba = sln::transform_pixels<sln::Pixel_8u4>(img_x, [](const auto& px) {
      return sln::convert_pixel<sln::PixelFormat::Y, sln::PixelFormat::RGBA>(px, std::uint8_t{0});
    });
    REQUIRE(img_rgba(0_idx, 0_idx) == sln::Pixel_8u4(10, 10, 10, 0));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::Pixel_8u4(20, 20, 20, 0));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::Pixel_8u4(30, 30, 30, 0));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::Pixel_8u4(40, 40, 40, 0));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::Pixel_8u4(50, 50, 50, 0));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::Pixel_8u4(60, 60, 60, 0));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::Pixel_8u4(70, 70, 70, 0));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::Pixel_8u4(80, 80, 80, 0));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::Pixel_8u4(90, 90, 90, 0));
  }

  SECTION("Convert RGB to BGR")
  {
    const auto img_bgr = sln::transform_pixels<sln::Pixel_8u3>(
        img_xxx, [](const auto& px) { return sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::BGR>(px); });
    REQUIRE(img_bgr(0_idx, 0_idx) == sln::Pixel_8u3(12, 11, 10));
    REQUIRE(img_bgr(1_idx, 0_idx) == sln::Pixel_8u3(22, 21, 20));
    REQUIRE(img_bgr(2_idx, 0_idx) == sln::Pixel_8u3(32, 31, 30));
    REQUIRE(img_bgr(0_idx, 1_idx) == sln::Pixel_8u3(42, 41, 40));
    REQUIRE(img_bgr(1_idx, 1_idx) == sln::Pixel_8u3(52, 51, 50));
    REQUIRE(img_bgr(2_idx, 1_idx) == sln::Pixel_8u3(62, 61, 60));
    REQUIRE(img_bgr(0_idx, 2_idx) == sln::Pixel_8u3(72, 71, 70));
    REQUIRE(img_bgr(1_idx, 2_idx) == sln::Pixel_8u3(82, 81, 80));
    REQUIRE(img_bgr(2_idx, 2_idx) == sln::Pixel_8u3(92, 91, 90));
  }

  SECTION("Convert RGB to Y")
  {
    sln::Image_8u1 img_y;
    sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_xxx, img_y);

    REQUIRE(img_y(0_idx, 0_idx) == 11);
    REQUIRE(img_y(1_idx, 0_idx) == 21);
    REQUIRE(img_y(2_idx, 0_idx) == 31);
    REQUIRE(img_y(0_idx, 1_idx) == 41);
    REQUIRE(img_y(1_idx, 1_idx) == 51);
    REQUIRE(img_y(2_idx, 1_idx) == 61);
    REQUIRE(img_y(0_idx, 2_idx) == 71);
    REQUIRE(img_y(1_idx, 2_idx) == 81);
    REQUIRE(img_y(2_idx, 2_idx) == 91);

    sln::Image_8u1 img_y_1 = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_xxx);
    REQUIRE(img_y_1 == img_y);
  }

  SECTION("Convert RGB to RGBA")
  {
    sln::Image_8u4 img_rgba;
    sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>(img_xxx, img_rgba, std::uint8_t{255});

    REQUIRE(img_rgba(0_idx, 0_idx) == sln::Pixel_8u4(10, 11, 12, 255));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::Pixel_8u4(20, 21, 22, 255));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::Pixel_8u4(30, 31, 32, 255));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::Pixel_8u4(40, 41, 42, 255));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::Pixel_8u4(50, 51, 52, 255));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::Pixel_8u4(60, 61, 62, 255));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::Pixel_8u4(70, 71, 72, 255));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::Pixel_8u4(80, 81, 82, 255));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::Pixel_8u4(90, 91, 92, 255));

    sln::Image_8u4 img_rgba_1 = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>(img_xxx,
                                                                                                  std::uint8_t{255});
    REQUIRE(img_rgba_1 == img_rgba);
  }
}
