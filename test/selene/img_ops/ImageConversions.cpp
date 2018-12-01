// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img_ops/ImageConversions.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img_ops/View.hpp>

#include <test/selene/img/typed/_Utils.hpp>

using namespace sln::literals;

TEST_CASE("Image conversions", "[img]")
{
  const auto img_x = sln_test::make_3x3_test_image_8u1();
  const auto img_xxx = sln_test::make_3x3_test_image_8u3();

  const auto img_y = sln::view<sln::Pixel_8u1, sln::PixelY_8u>(img_x);
  const auto img_rgb = sln::view<sln::Pixel_8u3, sln::PixelRGB_8u>(img_xxx);

  // Just covering a few select conversions for now...

  SECTION("Convert Y to RGBA (unknown source pixel format)")
  {
    const auto img_rgba = sln::transform_pixels<sln::PixelRGBA_8u>(img_x, [](const auto& px) {
      return sln::convert_pixel<sln::PixelFormat::Y, sln::PixelFormat::RGBA>(px, std::uint8_t{0});
    });
    REQUIRE(img_rgba(0_idx, 0_idx) == sln::PixelRGBA_8u(10, 10, 10, 0));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::PixelRGBA_8u(20, 20, 20, 0));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::PixelRGBA_8u(30, 30, 30, 0));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::PixelRGBA_8u(40, 40, 40, 0));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::PixelRGBA_8u(50, 50, 50, 0));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::PixelRGBA_8u(60, 60, 60, 0));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::PixelRGBA_8u(70, 70, 70, 0));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::PixelRGBA_8u(80, 80, 80, 0));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::PixelRGBA_8u(90, 90, 90, 0));
  }

  SECTION("Convert Y to RGBA (known source pixel format)")
  {
    const auto img_rgba = sln::transform_pixels<sln::PixelRGBA_8u>(img_y, [](const auto& px) {
      return sln::convert_pixel<sln::PixelFormat::RGBA>(px, std::uint8_t{0});
    });
    REQUIRE(img_rgba(0_idx, 0_idx) == sln::PixelRGBA_8u(10, 10, 10, 0));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::PixelRGBA_8u(20, 20, 20, 0));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::PixelRGBA_8u(30, 30, 30, 0));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::PixelRGBA_8u(40, 40, 40, 0));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::PixelRGBA_8u(50, 50, 50, 0));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::PixelRGBA_8u(60, 60, 60, 0));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::PixelRGBA_8u(70, 70, 70, 0));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::PixelRGBA_8u(80, 80, 80, 0));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::PixelRGBA_8u(90, 90, 90, 0));
  }

  SECTION("Convert RGB to BGR (unknown source pixel format)")
  {
    const auto img_bgr = sln::transform_pixels<sln::PixelBGR_8u>(
        img_xxx, [](const auto& px) { return sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::BGR>(px); });
    REQUIRE(img_bgr(0_idx, 0_idx) == sln::PixelBGR_8u(12, 11, 10));
    REQUIRE(img_bgr(1_idx, 0_idx) == sln::PixelBGR_8u(22, 21, 20));
    REQUIRE(img_bgr(2_idx, 0_idx) == sln::PixelBGR_8u(32, 31, 30));
    REQUIRE(img_bgr(0_idx, 1_idx) == sln::PixelBGR_8u(42, 41, 40));
    REQUIRE(img_bgr(1_idx, 1_idx) == sln::PixelBGR_8u(52, 51, 50));
    REQUIRE(img_bgr(2_idx, 1_idx) == sln::PixelBGR_8u(62, 61, 60));
    REQUIRE(img_bgr(0_idx, 2_idx) == sln::PixelBGR_8u(72, 71, 70));
    REQUIRE(img_bgr(1_idx, 2_idx) == sln::PixelBGR_8u(82, 81, 80));
    REQUIRE(img_bgr(2_idx, 2_idx) == sln::PixelBGR_8u(92, 91, 90));
  }

  SECTION("Convert RGB to BGR (known source pixel format)")
  {
    const auto img_bgr = sln::transform_pixels<sln::PixelBGR_8u>(
        img_rgb, [](const auto& px) { return sln::convert_pixel<sln::PixelFormat::BGR>(px); });
    REQUIRE(img_bgr(0_idx, 0_idx) == sln::PixelBGR_8u(12, 11, 10));
    REQUIRE(img_bgr(1_idx, 0_idx) == sln::PixelBGR_8u(22, 21, 20));
    REQUIRE(img_bgr(2_idx, 0_idx) == sln::PixelBGR_8u(32, 31, 30));
    REQUIRE(img_bgr(0_idx, 1_idx) == sln::PixelBGR_8u(42, 41, 40));
    REQUIRE(img_bgr(1_idx, 1_idx) == sln::PixelBGR_8u(52, 51, 50));
    REQUIRE(img_bgr(2_idx, 1_idx) == sln::PixelBGR_8u(62, 61, 60));
    REQUIRE(img_bgr(0_idx, 2_idx) == sln::PixelBGR_8u(72, 71, 70));
    REQUIRE(img_bgr(1_idx, 2_idx) == sln::PixelBGR_8u(82, 81, 80));
    REQUIRE(img_bgr(2_idx, 2_idx) == sln::PixelBGR_8u(92, 91, 90));
  }

  SECTION("Convert RGB to Y (unknown source pixel format)")
  {
    sln::ImageY_8u img_y_0;
    sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_xxx, img_y_0);

    REQUIRE(img_y_0(0_idx, 0_idx) == 11);
    REQUIRE(img_y_0(1_idx, 0_idx) == 21);
    REQUIRE(img_y_0(2_idx, 0_idx) == 31);
    REQUIRE(img_y_0(0_idx, 1_idx) == 41);
    REQUIRE(img_y_0(1_idx, 1_idx) == 51);
    REQUIRE(img_y_0(2_idx, 1_idx) == 61);
    REQUIRE(img_y_0(0_idx, 2_idx) == 71);
    REQUIRE(img_y_0(1_idx, 2_idx) == 81);
    REQUIRE(img_y_0(2_idx, 2_idx) == 91);

    sln::ImageY_8u img_y_1 = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_xxx);
    REQUIRE(img_y_1 == img_y_0);
  }

  SECTION("Convert RGB to Y (known source pixel format)")
  {
    sln::ImageY_8u img_y_0;
    sln::convert_image<sln::PixelFormat::Y>(img_rgb, img_y_0);

    REQUIRE(img_y_0(0_idx, 0_idx) == 11);
    REQUIRE(img_y_0(1_idx, 0_idx) == 21);
    REQUIRE(img_y_0(2_idx, 0_idx) == 31);
    REQUIRE(img_y_0(0_idx, 1_idx) == 41);
    REQUIRE(img_y_0(1_idx, 1_idx) == 51);
    REQUIRE(img_y_0(2_idx, 1_idx) == 61);
    REQUIRE(img_y_0(0_idx, 2_idx) == 71);
    REQUIRE(img_y_0(1_idx, 2_idx) == 81);
    REQUIRE(img_y_0(2_idx, 2_idx) == 91);

    sln::ImageY_8u img_y_1 = sln::convert_image<sln::PixelFormat::Y>(img_rgb);
    REQUIRE(img_y_1 == img_y_0);
  }

  SECTION("Convert RGB to RGBA (unknown source pixel format)")
  {
    sln::ImageRGBA_8u img_rgba;
    sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>(img_xxx, img_rgba, std::uint8_t{255});

    REQUIRE(img_rgba(0_idx, 0_idx) == sln::PixelRGBA_8u(10, 11, 12, 255));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::PixelRGBA_8u(20, 21, 22, 255));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::PixelRGBA_8u(30, 31, 32, 255));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::PixelRGBA_8u(40, 41, 42, 255));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::PixelRGBA_8u(50, 51, 52, 255));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::PixelRGBA_8u(60, 61, 62, 255));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::PixelRGBA_8u(70, 71, 72, 255));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::PixelRGBA_8u(80, 81, 82, 255));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::PixelRGBA_8u(90, 91, 92, 255));

    sln::ImageRGBA_8u img_rgba_1 = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>(img_xxx,
                                                                                                  std::uint8_t{255});
    REQUIRE(img_rgba_1 == img_rgba);
  }

  SECTION("Convert RGB to RGBA (known source pixel format)")
  {
    sln::ImageRGBA_8u img_rgba;
    sln::convert_image<sln::PixelFormat::RGBA>(img_rgb, img_rgba, std::uint8_t{255});

    REQUIRE(img_rgba(0_idx, 0_idx) == sln::PixelRGBA_8u(10, 11, 12, 255));
    REQUIRE(img_rgba(1_idx, 0_idx) == sln::PixelRGBA_8u(20, 21, 22, 255));
    REQUIRE(img_rgba(2_idx, 0_idx) == sln::PixelRGBA_8u(30, 31, 32, 255));
    REQUIRE(img_rgba(0_idx, 1_idx) == sln::PixelRGBA_8u(40, 41, 42, 255));
    REQUIRE(img_rgba(1_idx, 1_idx) == sln::PixelRGBA_8u(50, 51, 52, 255));
    REQUIRE(img_rgba(2_idx, 1_idx) == sln::PixelRGBA_8u(60, 61, 62, 255));
    REQUIRE(img_rgba(0_idx, 2_idx) == sln::PixelRGBA_8u(70, 71, 72, 255));
    REQUIRE(img_rgba(1_idx, 2_idx) == sln::PixelRGBA_8u(80, 81, 82, 255));
    REQUIRE(img_rgba(2_idx, 2_idx) == sln::PixelRGBA_8u(90, 91, 92, 255));

    sln::ImageRGBA_8u img_rgba_1 = sln::convert_image<sln::PixelFormat::RGBA>(img_rgb,
                                                                                                  std::uint8_t{255});
    REQUIRE(img_rgba_1 == img_rgba);
  }
}
