// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageView.hpp>
#include <selene/img/typed/ImageViewTypeAliases.hpp>

#include <selene/img_ops/Algorithms.hpp>
#include <selene/img_ops/Fill.hpp>
#include <selene/img_ops/Generate.hpp>
#include <selene/img_ops/Transformations.hpp>

#include <test/selene/img/typed/_Utils.hpp>

// ---
#include <iostream>

using namespace sln::literals;


TEST_CASE("Image view construction", "[img]")
{
  SECTION("From constant image")
  {
    const auto img = sln_test::make_3x3_test_image_8u3();
    const auto view = sln::ConstantImageViewRGB_8u{img.byte_ptr(), img.layout()};
    REQUIRE(view.width() == img.width());
    REQUIRE(view.height() == img.height());
    REQUIRE(view.stride_bytes() == img.stride_bytes());

    for (auto y = 0_idx; y < view.height(); ++y)
    {
      for (auto x = 0_idx; x < view.width(); ++x)
      {
        REQUIRE(view(x, y) == img(x, y));
      }
    }
  }

  SECTION("From non-constant image")
  {
    auto img = sln_test::make_3x3_test_image_8u3();
    auto view = sln::MutableImageViewRGB_8u{img.byte_ptr(), img.layout()};
    REQUIRE(view.width() == img.width());
    REQUIRE(view.height() == img.height());
    REQUIRE(view.stride_bytes() == img.stride_bytes());

    for (auto y = 0_idx; y < view.height(); ++y)
    {
      for (auto x = 0_idx; x < view.width(); ++x)
      {
        REQUIRE(view(x, y) == img(x, y));
      }
    }
  }
}

TEST_CASE("Image view modification", "[img]")
{
  auto img = sln_test::make_3x3_test_image_8u3();
  auto view = sln::MutableImageViewRGB_8u{img.byte_ptr(), img.layout()};
  REQUIRE(view.width() == img.width());
  REQUIRE(view.height() == img.height());
  REQUIRE(view.stride_bytes() == img.stride_bytes());

  view(1_idx, 1_idx) = sln::PixelRGB_8u{42, 0, 255};
  REQUIRE(img(1_idx, 1_idx) == sln::PixelRGB_8u{42, 0, 255});
}

TEST_CASE("Assigning an expression to an image view", "[img]")
{
  auto img = sln::generate([](auto x, auto y){ return sln::PixelY_16u{x + 12 * y}; }, 10_px, 20_px);
  auto view = sln::MutableImageViewY_16u{img.byte_ptr(), img.layout()};
  REQUIRE(view.width() == img.width());
  REQUIRE(view.height() == img.height());
  REQUIRE(view.stride_bytes() == img.stride_bytes());

  for (auto y = 0_idx; y < view.height(); ++y)
  {
    for (auto x = 0_idx; x < view.width(); ++x)
    {
      REQUIRE(view(x, y) == sln::PixelY_16u{x + 12 * y});
    }
  }

  const auto transf_func = [](const auto& px){ return px + sln::PixelY_16u{47}; };
  const auto img2 = sln::transform_pixels<sln::PixelY_16u>(img, transf_func);

  view = sln::transform_pixels_expr(img, transf_func);
  REQUIRE(view.width() == img2.width());
  REQUIRE(view.height() == img2.height());

  for (auto y = 0_idx; y < view.height(); ++y)
  {
    for (auto x = 0_idx; x < view.width(); ++x)
    {
      REQUIRE(view(x, y) == img2(x, y));
    }
  }
}