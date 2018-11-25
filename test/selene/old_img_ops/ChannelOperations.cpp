// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/old_img/ImageTypeAliases.hpp>
#include <selene/old_img/PixelTraits.hpp>
#include <selene/old_img_ops/ChannelOperations.hpp>

using namespace sln::literals;

constexpr auto w_test = 3_px;
constexpr auto h_test = 4_px;

constexpr std::uint8_t val_r = 64;
constexpr std::uint8_t val_g = 255;
constexpr std::uint8_t val_b = 128;

constexpr std::array<std::uint8_t, 3> values_rgb = {{val_r, val_g, val_b}};

namespace {

template <std::size_t N, typename Img>
void check_channels(const Img& img, const std::array<std::uint8_t, N>& values)
{
  constexpr auto nr_channels = sln::PixelTraits<typename Img::PixelType>::nr_channels;
  static_assert(nr_channels == N, "nr channels mismatch");

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      for (auto c = 0ul; c < N; ++c)
      {
        REQUIRE(img(x, y)[c] == values.at(c));
      }
    }
  }

}

} // namespace


TEST_CASE("Channel injection", "[old_img]")
{
  sln::Image_8u1 img_r(w_test, h_test);
  sln::Image_8u1 img_g(w_test, h_test);
  sln::Image_8u1 img_b(w_test, h_test);
  img_r.fill(val_r);
  img_g.fill(val_g);
  img_b.fill(val_b);

  SECTION("Inject in RGB image")
  {
    sln::ImageRGB_8u img_rgb(w_test, h_test);
    sln::inject_channels(img_r, img_rgb, 0);
    sln::inject_channels(img_g, img_rgb, 1);
    sln::inject_channels(img_b, img_rgb, 2);
    check_channels(img_rgb, values_rgb);
  }

}

TEST_CASE("Channel stacking", "[old_img]")
{
  sln::Image_8u1 img_r(w_test, h_test);
  sln::Image_8u1 img_g(w_test, h_test);
  sln::Image_8u1 img_b(w_test, h_test);
  img_r.fill(val_r);
  img_g.fill(val_g);
  img_b.fill(val_b);

  SECTION("Stack to RGB image")
  {
    const auto img_rgb = sln::stack_images<sln::PixelFormat::RGB>(img_r, img_g, img_b);
    check_channels(img_rgb, values_rgb);
  }

  SECTION("Stack to 3-channel image")
  {
    const auto img_rgb = sln::stack_images(img_r, img_g, img_b);
    check_channels(img_rgb, values_rgb);
  }

  SECTION("Stack to RGBA image")
  {
    const auto img_rgba = sln::stack_images<sln::PixelFormat::RGBA>(img_r, img_g, img_b, img_g);
    check_channels<4>(img_rgba, {{val_r, val_g, val_b, val_g}});
  }

  SECTION("Stack to 4-channel image")
  {
    const auto img_rgba = sln::stack_images(img_r, img_g, img_b, img_g);
    check_channels<4>(img_rgba, {{val_r, val_g, val_b, val_g}});
  }

  SECTION("Stack up to 6-channel image")
  {
    const auto img_rgb = sln::stack_images(img_r, img_g, img_b);
    REQUIRE(img_rgb.width() == w_test);
    REQUIRE(img_rgb.height() == h_test);
    check_channels<3>(img_rgb, values_rgb);

    const auto img_br = sln::stack_images(img_b, img_r);
    REQUIRE(img_br.width() == w_test);
    REQUIRE(img_br.height() == h_test);
    check_channels<2>(img_br, {{val_b, val_r}});

    const auto img_5 = sln::stack_images(img_rgb, img_br);
    REQUIRE(img_5.width() == w_test);
    REQUIRE(img_5.height() == h_test);
    check_channels<5>(img_5, {{val_r, val_g, val_b, val_b, val_r}});

    const auto img_6 = sln::stack_images(img_rgb, img_br, img_b);
    REQUIRE(img_6.width() == w_test);
    REQUIRE(img_6.height() == h_test);
    check_channels<6>(img_6, {{val_r, val_g, val_b, val_b, val_r, val_b}});
  }
}
