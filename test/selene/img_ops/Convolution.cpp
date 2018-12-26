// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/Convolution.hpp>

#include <selene/base/Kernel.hpp>
#include <selene/base/io/FileReader.hpp>

#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img_io/IO.hpp>

#include <test/selene/img/typed/_Utils.hpp>

#include <boost/filesystem.hpp>

using namespace sln::literals;
namespace fs = boost::filesystem;

namespace {

fs::path full_path(const char* filename)
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? (fs::path(env_var) / fs::path(filename)) : (fs::path("../data") / fs::path(filename));
}

}  // namespace

TEST_CASE("Convolution (pixels)", "[img]")
{
  sln::Kernel<double, 3> k{{0.3, 0.5, 0.2}};

  SECTION("X-direction")
  {
    sln::Image_8u1 img({3_px, 1_px});
    img(0_idx, 0_idx) = 30;
    img(1_idx, 0_idx) = 10;
    img(2_idx, 0_idx) = 80;

    const auto res0 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::Unchecked>(img, 1_idx, 0_idx, k, 1);
    REQUIRE(res0 == Approx(30.0));

    const auto res1 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, k, 1);
    REQUIRE(res1 == Approx(26.0));

    const auto res2 = sln::impl::convolve_pixels_x<double, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, k, 1);
    REQUIRE(res2 == Approx(17.0));
  }

  SECTION("Y-direction")
  {
    sln::Image_8u1 img({1_px, 3_px});
    img(0_idx, 0_idx) = 30;
    img(0_idx, 1_idx) = 10;
    img(0_idx, 2_idx) = 80;

    const auto res0 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::Unchecked>(img, 0_idx, 1_idx, k, 1);
    REQUIRE(res0 == Approx(30.0));

    const auto res1 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::Replicated>(img, 0_idx, 0_idx, k, 1);
    REQUIRE(res1 == Approx(26.0));

    const auto res2 = sln::impl::convolve_pixels_y<double, sln::BorderAccessMode::ZeroPadding>(img, 0_idx, 0_idx, k, 1);
    REQUIRE(res2 == Approx(17.0));
  }
};

TEST_CASE("Image convolution", "[img]")
{
  // TODO: test convolution on some easier cases, without relying on image IO?
  const auto img_path = full_path("stickers.png");
  sln::FileReader source(img_path.string());
  auto dyn_img = sln::read_image(source);
  const auto img_src = sln::to_image<sln::PixelRGB_8u>(std::move(dyn_img));
  REQUIRE(img_src.is_valid());

  const auto kernel = sln::gaussian_kernel<7>(2.0);
  sln::ImageRGB_8u img_dst;

  SECTION("X-direction")
  {
    sln::convolution_x(img_src, img_dst, kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(158, 184, 187));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 238, 230));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(161, 154, 145));
  }

  SECTION("Y-direction")
  {
    sln::convolution_y(img_src, img_dst, kernel);
    REQUIRE(img_dst.width() == img_src.width());
    REQUIRE(img_dst.height() == img_src.height());

//    auto print_coords = [&img_dst](auto x, auto y){
//      std::cout << "img_dst(" << x << ", " << y << " = " << img_dst(x, y) << '\n';
//    };
//    print_coords(100_idx, 100_idx);
//    print_coords(200_idx, 200_idx);
//    print_coords(300_idx, 300_idx);

    REQUIRE(img_dst(100_idx, 100_idx) == sln::PixelRGB_8u(149, 183, 189));
    REQUIRE(img_dst(200_idx, 200_idx) == sln::PixelRGB_8u(245, 239, 232));
    REQUIRE(img_dst(300_idx, 300_idx) == sln::PixelRGB_8u(162, 151, 143));
  }
}
