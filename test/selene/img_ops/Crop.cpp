// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Crop.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

using namespace sln::literals;

namespace {

auto px_value = [](sln::PixelIndex x, sln::PixelIndex y) {
  return static_cast<std::uint8_t>(x + 2 * y);
};

auto generate_image()
{
  sln::Image_8u1 img({12_px, 12_px});

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      img(x, y) = px_value(x, y);
    }
  }

  return img;
}

}  // namespace _

TEST_CASE("Image cropping", "[img]")
{
  const auto img = generate_image();

  SECTION("Using crop function")
  {
    for (auto bb_y = 0_idx; bb_y < img.height(); ++bb_y)
    {
      for (auto bb_x = 0_idx; bb_x < img.width(); ++bb_x)
      {
        for (auto bb_h = 0_px; bb_h < img.height() - bb_y; ++bb_h)
        {
          for (auto bb_w = 0_px; bb_w < img.width() - bb_x; ++bb_w)
          {
            auto img_tmp = img;
            sln::crop(img_tmp, sln::BoundingBox{bb_x, bb_y, bb_w, bb_h});
            REQUIRE(img_tmp.width() == bb_w);
            REQUIRE(img_tmp.height() == bb_h);

            for (auto y = 0_idx; y < img_tmp.height(); ++y)
            {
              for (auto x = 0_idx; x < img_tmp.width(); ++x)
              {
                REQUIRE(img_tmp(x, y) == px_value(x + bb_x, y + bb_y));
              }
            }

          }
        }
      }
    }
  }

  SECTION("Using crop_expr function")
  {
    for (auto bb_y = 0_idx; bb_y < img.height(); ++bb_y)
    {
      for (auto bb_x = 0_idx; bb_x < img.width(); ++bb_x)
      {
        for (auto bb_h = 0_px; bb_h < img.height() - bb_y; ++bb_h)
        {
          for (auto bb_w = 0_px; bb_w < img.width() - bb_x; ++bb_w)
          {
            auto crop_expr = sln::crop_expr(img, sln::BoundingBox{bb_x, bb_y, bb_w, bb_h});
            REQUIRE(crop_expr.width() == bb_w);
            REQUIRE(crop_expr.height() == bb_h);

            for (auto y = 0_idx; y < crop_expr.height(); ++y)
            {
              for (auto x = 0_idx; x < crop_expr.width(); ++x)
              {
                REQUIRE(crop_expr(x, y) == px_value(x + bb_x, y + bb_y));
              }
            }

            const auto img_crop = crop_expr.eval();
            REQUIRE(img_crop.width() == bb_w);
            REQUIRE(img_crop.height() == bb_h);

            for (auto y = 0_idx; y < img_crop.height(); ++y)
            {
              for (auto x = 0_idx; x < img_crop.width(); ++x)
              {
                REQUIRE(img_crop(x, y) == px_value(x + bb_x, y + bb_y));
              }
            }

          }
        }
      }
    }
  }
}
