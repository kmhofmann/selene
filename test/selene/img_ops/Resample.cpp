// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Resample.hpp>

#include <selene/img/typed/access/Interpolators.hpp>

#include <test/selene/img/typed/_Utils.hpp>

#include <random>

using namespace sln::literals;

TEST_CASE("Image resampling", "[img]")
{
  const auto img = sln_test::make_3x3_test_image_8u1();

  SECTION("Nearest neighbor, doubling")
  {
    const auto img_r = sln::resample<sln::ImageInterpolationMode::NearestNeighbor>(img, 6_px, 6_px);
    REQUIRE(img_r.width() == 6);
    REQUIRE(img_r.height() == 6);

    for (auto y = 0_idx; y < img_r.height(); ++y)
    {
      for (auto x = 0_idx; x < img_r.width(); ++x)
      {
        REQUIRE(img_r(x, y) == img(sln::PixelIndex{x / 2}, sln::PixelIndex{y / 2}));
      }
    }
  }

  SECTION("Bilinear, doubling")
  {
    const auto img_r = sln::resample<sln::ImageInterpolationMode::Bilinear>(img, 6_px, 6_px);
    REQUIRE(img_r.width() == 6);
    REQUIRE(img_r.height() == 6);

    // clang-format off
    std::array<std::uint8_t, 36> values = {{
        10, 15, 20, 25, 30, 30,
        25, 30, 35, 40, 45, 45,
        40, 45, 50, 55, 60, 60,
        55, 60, 65, 70, 75, 75,
        70, 75, 80, 85, 90, 90,
        70, 75, 80, 85, 90, 90}};
    // clang-format on

    for (auto y = 0_idx; y < img_r.height(); ++y)
    {
      for (auto x = 0_idx; x < img_r.width(); ++x)
      {
        const auto i = std::size_t(y * img_r.width() + x);
        REQUIRE(img_r(x, y) == values[i]);
      }
    }
  }

  SECTION("Bilinear, random")
  {
    std::mt19937 rng{42};
    for (int i = 0; i < 100; ++i)
    {
      const auto rand_img = sln_test::construct_random_image<sln::Pixel<std::uint8_t, 3>>(10_px, 10_px, rng);
      const auto img_r = sln::resample<sln::ImageInterpolationMode::Bilinear>(img, 21_px, 33_px);
      REQUIRE(img_r.width() == 21_px);
      REQUIRE(img_r.height() == 33_px);
    }
  }
}
