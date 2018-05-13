// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img_ops/Transformations.hpp>

#include <random>

#include <test/img/_TestImages.hpp>

using namespace sln::literals;

TEST_CASE("Image transformations", "[img]")
{
  std::mt19937 rng(100);
  std::uniform_int_distribution<sln::PixelIndex::value_type> dist_size(0, 64);

  for (std::size_t count = 0; count < 32; ++count)
  {
    const auto width = sln::PixelLength{dist_size(rng)};
    const auto height = sln::PixelLength{dist_size(rng)};
    const auto img = sln_test::make_random_image<sln::Pixel_8u3>(width, height, rng);
    REQUIRE(img.width() == width);
    REQUIRE(img.height() == height);

    // transpose

    const auto img_transp = sln::transpose(img);
    REQUIRE(sln::transpose(img_transp) == img);
    REQUIRE(img_transp.width() == img.height());
    REQUIRE(img_transp.height() == img.width());

    for (auto y = 0_idx; y < img_transp.height(); ++y)
    {
      for (auto x = 0_idx; x < img_transp.width(); ++x)
      {
        REQUIRE(img_transp(x, y) == img(y, x));
      }
    }

    // flip horizontal

    const auto img_flip_h = sln::flip<sln::FlipDirection::Horizontal>(img);
    REQUIRE(sln::flip<sln::FlipDirection::Horizontal>(img_flip_h) == img);
    REQUIRE(img_flip_h.width() == img.width());
    REQUIRE(img_flip_h.height() == img.height());

    for (auto y = 0_idx; y < img_flip_h.height(); ++y)
    {
      for (auto x = 0_idx; x < img_flip_h.width(); ++x)
      {
        const auto px = sln::PixelIndex{img.width() - x - 1};
        REQUIRE(img_flip_h(x, y) == img(px, y));
      }
    }

    auto img_flip_h_in_place = clone(img);
    sln::flip_horizontally_in_place(img_flip_h_in_place);
    REQUIRE(img_flip_h_in_place == img_flip_h);

    // flip vertical

    const auto img_flip_v = sln::flip<sln::FlipDirection::Vertical>(img);
    REQUIRE(sln::flip<sln::FlipDirection::Vertical>(img_flip_v) == img);
    REQUIRE(img_flip_v.width() == img.width());
    REQUIRE(img_flip_v.height() == img.height());

    for (auto y = 0_idx; y < img_flip_v.height(); ++y)
    {
      for (auto x = 0_idx; x < img_flip_v.width(); ++x)
      {
        const auto py = sln::PixelIndex{img.height() - y - 1};
        REQUIRE(img_flip_v(x, y) == img(x, py));
      }
    }

    auto img_flip_v_in_place = clone(img);
    sln::flip_vertically_in_place(img_flip_v_in_place);
    REQUIRE(img_flip_v_in_place == img_flip_v);

    // flip both

    const auto img_flip_b = sln::flip<sln::FlipDirection::Both>(img);
    REQUIRE(sln::flip<sln::FlipDirection::Both>(img_flip_b) == img);
    REQUIRE(img_flip_b.width() == img.width());
    REQUIRE(img_flip_b.height() == img.height());

    for (auto y = 0_idx; y < img_flip_b.height(); ++y)
    {
      for (auto x = 0_idx; x < img_flip_b.width(); ++x)
      {
        const auto px = sln::PixelIndex{img.width() - x - 1};
        const auto py = sln::PixelIndex{img.height() - y - 1};
        REQUIRE(img_flip_b(x, y) == img(px, py));
      }
    }

    // rotate

    REQUIRE(sln::rotate<sln::RotationDirection::Clockwise0>(img)
            == sln::rotate<sln::RotationDirection::Counterclockwise0>(img));
    REQUIRE(sln::rotate<sln::RotationDirection::Clockwise90>(img)
            == sln::rotate<sln::RotationDirection::Counterclockwise270>(img));
    REQUIRE(sln::rotate<sln::RotationDirection::Clockwise180>(img)
            == sln::rotate<sln::RotationDirection::Counterclockwise180>(img));
    REQUIRE(sln::rotate<sln::RotationDirection::Clockwise270>(img)
            == sln::rotate<sln::RotationDirection::Counterclockwise90>(img));
  }
}
