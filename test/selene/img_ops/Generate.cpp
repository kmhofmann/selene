// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Generate.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

using namespace sln::literals;

namespace {

std::uint8_t generate_image(sln::PixelIndex x, sln::PixelIndex y)
{
  return static_cast<std::uint8_t>(x * x + 50 * y);
}

}  // namespace _

TEST_CASE("Image generation", "[img]")
{
  SECTION("Using generate function")
  {
    sln::ImageY_8u img{{20_px, 10_px}};
    sln::generate(generate_image, img);
    REQUIRE(img.width() == 20_px);
    REQUIRE(img.height() == 10_px);

    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == sln::PixelY_8u{x * x + 50 * y});
      }
    }
  }

  SECTION("Using generate_expr function")
  {
    const auto expr = sln::generate_expr(&generate_image, 20_px, 10_px);
    REQUIRE(expr.width() == 20_px);
    REQUIRE(expr.height() == 10_px);

    for (auto y = 0_idx; y < expr.height(); ++y)
    {
      for (auto x = 0_idx; x < expr.width(); ++x)
      {
        REQUIRE(expr(x, y) == sln::PixelY_8u{x * x + 50 * y});
      }
    }

    const auto img = expr.eval();
    REQUIRE(img.width() == 20_px);
    REQUIRE(img.height() == 10_px);

    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == sln::PixelY_8u{x * x + 50 * y});
      }
    }
  }
}