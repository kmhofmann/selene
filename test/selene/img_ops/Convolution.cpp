// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Convolution.hpp>

#include <selene/base/Kernel.hpp>
#include <selene/base/io/FileReader.hpp>

#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img_io/IO.hpp>

#include <test/selene/Utils.hpp>

using namespace sln::literals;

TEST_CASE("Convolution (pixels)", "[img]")
{
  const sln::Kernel<double, 3> k{{0.3, 0.5, 0.2}};
  constexpr auto k_offset = sln::PixelIndex::value_type{1};

  constexpr auto shift = 8u;
  const auto ik = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(k);
  REQUIRE(ik[0] == 77);
  REQUIRE(ik[1] == 128);
  REQUIRE(ik[2] == 51);

  SECTION("X-direction")
  {
    sln::Image_8u1 img({3_px, 1_px});
    img(0_idx, 0_idx) = 30;
    img(1_idx, 0_idx) = 10;
    img(2_idx, 0_idx) = 80;

    // floating point kernel
    {
      const auto res0 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::Unchecked>(img, 1_idx, 0_idx, k, k_offset);
      REQUIRE(res0 == Approx(30.0));
      const auto res1 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, k, k_offset);
      REQUIRE(res1 == Approx(26.0));
      const auto res2 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, k, k_offset);
      REQUIRE(res2 == Approx(17.0));

      sln::Image_8u1 img_dst = sln::convolution_x<sln::BorderAccessMode::Replicated>(img, k);
      REQUIRE(img_dst(0_idx, 0_idx) == 26);
      REQUIRE(img_dst(1_idx, 0_idx) == 30);
      REQUIRE(img_dst(2_idx, 0_idx) == 59);
    }
    // integer kernel
    {
      const auto res0 = sln::impl::convolve_pixels_x<std::int32_t, sln::BorderAccessMode::Unchecked>(img, 1_idx, 0_idx, ik, k_offset);
      REQUIRE(res0 == 7670);
      const auto res1 = sln::impl::convolve_pixels_x<std::int32_t, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, ik, k_offset);
      REQUIRE(res1 == 6660);
      const auto res2 = sln::impl::convolve_pixels_x<std::int32_t, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, ik, k_offset);
      REQUIRE(res2 == 4350);

      sln::Image_8u1 img_dst = sln::convolution_x<sln::BorderAccessMode::Replicated, shift>(img, ik);
      REQUIRE(img_dst(0_idx, 0_idx) == 26);
      REQUIRE(img_dst(1_idx, 0_idx) == 30);
      REQUIRE(img_dst(2_idx, 0_idx) == 59);
    }
  }

  SECTION("Y-direction")
  {
    sln::Image_8u1 img({1_px, 3_px});
    img(0_idx, 0_idx) = 30;
    img(0_idx, 1_idx) = 10;
    img(0_idx, 2_idx) = 80;

    // floating point kernel
    {
      const auto res0 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::Unchecked>(img, 0_idx, 1_idx, k, k_offset);
      REQUIRE(res0 == Approx(30.0));
      const auto res1 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, k, k_offset);
      REQUIRE(res1 == Approx(26.0));
      const auto res2 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, k, k_offset);
      REQUIRE(res2 == Approx(17.0));

      sln::Image_8u1 img_dst = sln::convolution_y<sln::BorderAccessMode::Replicated>(img, k);
      REQUIRE(img_dst(0_idx, 0_idx) == 26);
      REQUIRE(img_dst(0_idx, 1_idx) == 30);
      REQUIRE(img_dst(0_idx, 2_idx) == 59);
    }
    // integer kernel
    {
      const auto res0 = sln::impl::convolve_pixels_y<std::int32_t, sln::BorderAccessMode::Unchecked>(img, 0_idx, 1_idx, ik, k_offset);
      REQUIRE(res0 == 7670);
      const auto res1 = sln::impl::convolve_pixels_y<std::int32_t, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, ik, k_offset);
      REQUIRE(res1 == 6660);
      const auto res2 = sln::impl::convolve_pixels_y<std::int32_t, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, ik, k_offset);
      REQUIRE(res2 == 4350);

      sln::Image_8u1 img_dst = sln::convolution_y<sln::BorderAccessMode::Replicated, shift>(img, ik);
      REQUIRE(img_dst(0_idx, 0_idx) == 26);
      REQUIRE(img_dst(0_idx, 1_idx) == 30);
      REQUIRE(img_dst(0_idx, 2_idx) == 59);
    }
  }
}

TEST_CASE("Image convolution (IO)", "[img]")
{
  // TODO: test convolution on some easier cases, without relying on image IO?
  const auto img_path = sln_test::full_data_path("stickers.png");
  sln::FileReader source(img_path.string());
  auto dyn_img = sln::read_image(source);
  const auto img_src = sln::to_image<sln::PixelRGB_8u>(std::move(dyn_img));
  REQUIRE(img_src.is_valid());

  const auto kernel = sln::gaussian_kernel<7>(2.0);

  constexpr auto shift = 16u;
  const auto integral_kernel = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel);

  sln::ImageRGB_8u img_dst;

  SECTION("X-direction")
  {
    sln::convolution_x<sln::BorderAccessMode::Replicated>(img_src, img_dst, kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

//    auto print_coords = [&img_dst](auto x, auto y){
//      std::cout << "img_dst(" << x << ", " << y << ") = " << img_dst(x, y) << '\n';
//    };
//    print_coords(100_idx, 100_idx);
//    print_coords(200_idx, 200_idx);
//    print_coords(300_idx, 300_idx);

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(158, 184, 187));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 238, 230));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(161, 154, 145));
  }

  SECTION("X-direction, integral")
  {
    sln::convolution_x<sln::BorderAccessMode::Replicated, shift>(img_src, img_dst, integral_kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(158, 184, 187));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 238, 230));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(161, 154, 145));
  }

  SECTION("Y-direction")
  {
    sln::convolution_y<sln::BorderAccessMode::Replicated>(img_src, img_dst, kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(149, 183, 189));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 239, 232));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(162, 151, 143));
  }

  SECTION("Y-direction, integral")
  {
    sln::convolution_y<sln::BorderAccessMode::Replicated, shift>(img_src, img_dst, integral_kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(149, 183, 189));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 239, 232));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(162, 151, 143));
  }
}
