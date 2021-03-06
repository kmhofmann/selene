// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img_ops/Transformations.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img_ops/Clone.hpp>

#include <random>

#include <test/selene/img/typed/_Utils.hpp>

using namespace sln::literals;

template <typename Img> void test_flip(const Img& img)
{
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

  auto img_flip_h_in_place = sln::clone(img);
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

  auto img_flip_v_in_place = sln::clone(img);
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
}

template <typename Img> void test_transpose(const Img& img)
{
  const auto img_transp = sln::transpose(img);
  REQUIRE(img_transp.width() == img.height());
  REQUIRE(img_transp.height() == img.width());
  REQUIRE(sln::transpose(img_transp) == img);

  for (auto y = 0_idx; y < img_transp.height(); ++y)
  {
    for (auto x = 0_idx; x < img_transp.width(); ++x)
    {
      REQUIRE(img_transp(x, y) == img(y, x));
    }
  }
}

template <typename Img> void test_rotate(const Img& img)
{
  REQUIRE(sln::rotate<sln::RotationDirection::Clockwise0>(img)
          == sln::rotate<sln::RotationDirection::Counterclockwise0>(img));
  REQUIRE(sln::rotate<sln::RotationDirection::Clockwise90>(img)
          == sln::rotate<sln::RotationDirection::Counterclockwise270>(img));
  REQUIRE(sln::rotate<sln::RotationDirection::Clockwise180>(img)
          == sln::rotate<sln::RotationDirection::Counterclockwise180>(img));
  REQUIRE(sln::rotate<sln::RotationDirection::Clockwise270>(img)
          == sln::rotate<sln::RotationDirection::Counterclockwise90>(img));
}

TEST_CASE("Image transformations / random images", "[img]")
{
  std::mt19937 rng(100);
  std::uniform_int_distribution<sln::PixelIndex::value_type> dist_size(2, 4);

  for (std::size_t count = 0; count < 32; ++count)
  {
    const auto width = sln::PixelLength{dist_size(rng)};
    const auto height = sln::PixelLength{dist_size(rng)};
    const auto img = sln_test::construct_random_image<sln::Pixel_8u3>(width, height, rng);
    REQUIRE(img.width() == width);
    REQUIRE(img.height() == height);

    test_flip(img);
    test_transpose(img);
    test_rotate(img);
  }
}

TEST_CASE("Image transformation expressions", "[img]")
{
  sln::ImageY_8u img({3_px, 2_px});
  img(0_idx, 0_idx) = 10;
  img(1_idx, 0_idx) = 20;
  img(2_idx, 0_idx) = 30;
  img(0_idx, 1_idx) = 40;
  img(1_idx, 1_idx) = 50;
  img(2_idx, 1_idx) = 60;

  SECTION("Flip")
  {
    auto flip_h_expr = sln::flip_expr<sln::FlipDirection::Horizontal>(img);
    REQUIRE(flip_h_expr(0_idx, 0_idx) == 30);
    REQUIRE(flip_h_expr(1_idx, 0_idx) == 20);
    REQUIRE(flip_h_expr(2_idx, 0_idx) == 10);
    REQUIRE(flip_h_expr(0_idx, 1_idx) == 60);
    REQUIRE(flip_h_expr(1_idx, 1_idx) == 50);
    REQUIRE(flip_h_expr(2_idx, 1_idx) == 40);

    auto flip_v_expr = sln::flip_expr<sln::FlipDirection::Vertical>(img);
    REQUIRE(flip_v_expr(0_idx, 0_idx) == 40);
    REQUIRE(flip_v_expr(1_idx, 0_idx) == 50);
    REQUIRE(flip_v_expr(2_idx, 0_idx) == 60);
    REQUIRE(flip_v_expr(0_idx, 1_idx) == 10);
    REQUIRE(flip_v_expr(1_idx, 1_idx) == 20);
    REQUIRE(flip_v_expr(2_idx, 1_idx) == 30);

    auto flip_b_expr = sln::flip_expr<sln::FlipDirection::Both>(img);
    REQUIRE(flip_b_expr(0_idx, 0_idx) == 60);
    REQUIRE(flip_b_expr(1_idx, 0_idx) == 50);
    REQUIRE(flip_b_expr(2_idx, 0_idx) == 40);
    REQUIRE(flip_b_expr(0_idx, 1_idx) == 30);
    REQUIRE(flip_b_expr(1_idx, 1_idx) == 20);
    REQUIRE(flip_b_expr(2_idx, 1_idx) == 10);
  }

  SECTION("Transpose")
  {
    auto transp_expr = sln::transpose_expr<false, false>(img);
    REQUIRE(transp_expr.width() == img.height());
    REQUIRE(transp_expr.height() == img.width());
    REQUIRE(transp_expr(0_idx, 0_idx) == 10);
    REQUIRE(transp_expr(1_idx, 0_idx) == 40);
    REQUIRE(transp_expr(0_idx, 1_idx) == 20);
    REQUIRE(transp_expr(1_idx, 1_idx) == 50);
    REQUIRE(transp_expr(0_idx, 2_idx) == 30);
    REQUIRE(transp_expr(1_idx, 2_idx) == 60);
  }

  SECTION("Rotate")
  {
    auto rot_0_expr = sln::rotate_expr<sln::RotationDirection::Clockwise0>(img);
    REQUIRE(rot_0_expr.width() == img.width());
    REQUIRE(rot_0_expr.height() == img.height());
    REQUIRE(rot_0_expr(0_idx, 0_idx) == 10);
    REQUIRE(rot_0_expr(1_idx, 0_idx) == 20);
    REQUIRE(rot_0_expr(2_idx, 0_idx) == 30);
    REQUIRE(rot_0_expr(0_idx, 1_idx) == 40);
    REQUIRE(rot_0_expr(1_idx, 1_idx) == 50);
    REQUIRE(rot_0_expr(2_idx, 1_idx) == 60);

    auto rot_90_expr = sln::rotate_expr<sln::RotationDirection::Clockwise90>(img);
    REQUIRE(rot_90_expr.width() == img.height());
    REQUIRE(rot_90_expr.height() == img.width());
    REQUIRE(rot_90_expr(0_idx, 0_idx) == 40);
    REQUIRE(rot_90_expr(1_idx, 0_idx) == 10);
    REQUIRE(rot_90_expr(0_idx, 1_idx) == 50);
    REQUIRE(rot_90_expr(1_idx, 1_idx) == 20);
    REQUIRE(rot_90_expr(0_idx, 2_idx) == 60);
    REQUIRE(rot_90_expr(1_idx, 2_idx) == 30);

    auto rot_180_expr = sln::rotate_expr<sln::RotationDirection::Clockwise180>(img);
    REQUIRE(rot_180_expr.width() == img.width());
    REQUIRE(rot_180_expr.height() == img.height());
    REQUIRE(rot_180_expr(0_idx, 0_idx) == 60);
    REQUIRE(rot_180_expr(1_idx, 0_idx) == 50);
    REQUIRE(rot_180_expr(2_idx, 0_idx) == 40);
    REQUIRE(rot_180_expr(0_idx, 1_idx) == 30);
    REQUIRE(rot_180_expr(1_idx, 1_idx) == 20);
    REQUIRE(rot_180_expr(2_idx, 1_idx) == 10);

    auto rot_270_expr = sln::rotate_expr<sln::RotationDirection::Clockwise270>(img);
    REQUIRE(rot_270_expr.width() == img.height());
    REQUIRE(rot_270_expr.height() == img.width());
    REQUIRE(rot_270_expr(0_idx, 0_idx) == 30);
    REQUIRE(rot_270_expr(1_idx, 0_idx) == 60);
    REQUIRE(rot_270_expr(0_idx, 1_idx) == 20);
    REQUIRE(rot_270_expr(1_idx, 1_idx) == 50);
    REQUIRE(rot_270_expr(0_idx, 2_idx) == 10);
    REQUIRE(rot_270_expr(1_idx, 2_idx) == 40);
  }
}