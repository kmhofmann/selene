// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Utils.hpp>

#include <selene/img/Algorithms.hpp>
#include <selene/img/PixelConversions.hpp>

#include <limits>
#include <random>

#include "_TestImages.hpp"

using namespace sln::literals;

template <typename T>
void check_pixel_conversions(T upper_bound = std::numeric_limits<T>::max(),
                             T lower_bound = std::numeric_limits<T>::min())
{
  for (T x = lower_bound; x != upper_bound; ++x)
  {
    sln::Pixel<T, 3> src_xxx(x, x, x);
    sln::Pixel<T, 4> src_xxxa(x, x, x, 0);
    sln::Pixel<T, 4> src_axxx(0, x, x, x);

    const auto dst_rgb_to_y = sln::rgb_to_y(src_xxx);
    REQUIRE(static_cast<std::uint64_t>(dst_rgb_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_bgr_to_y = sln::bgr_to_y(src_xxx);
    REQUIRE(static_cast<std::uint64_t>(dst_bgr_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_rgba_to_y = sln::rgba_to_y(src_xxxa);
    REQUIRE(static_cast<std::uint64_t>(dst_rgba_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_bgra_to_y = sln::bgra_to_y(src_xxxa);
    REQUIRE(static_cast<std::uint64_t>(dst_rgba_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_argb_to_y = sln::argb_to_y(src_axxx);
    REQUIRE(static_cast<std::uint64_t>(dst_argb_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_abgr_to_y = sln::abgr_to_y(src_axxx);
    REQUIRE(static_cast<std::uint64_t>(dst_abgr_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_rgb_to_ya = sln::rgb_to_ya(src_xxx, T{0});
    REQUIRE(dst_rgb_to_ya == sln::Pixel<T, 2>(x, 0));

    const auto dst_bgr_to_ya = sln::bgr_to_ya(src_xxx, T{0});
    REQUIRE(dst_bgr_to_ya == sln::Pixel<T, 2>(x, 0));

    const auto dst_rgba_to_ya = sln::rgba_to_ya(src_xxxa);
    REQUIRE(dst_rgba_to_ya == sln::Pixel<T, 2>(x, 0));

    const auto dst_bgra_to_ya = sln::bgra_to_ya(src_xxxa);
    REQUIRE(dst_rgba_to_ya == sln::Pixel<T, 2>(x, 0));

    const auto dst_argb_to_ya = sln::argb_to_ya(src_axxx);
    REQUIRE(dst_argb_to_ya == sln::Pixel<T, 2>(x, 0));

    const auto dst_abgr_to_ya = sln::abgr_to_ya(src_axxx);
    REQUIRE(dst_abgr_to_ya == sln::Pixel<T, 2>(x, 0));

    sln::Pixel<T, 3> src1(x / 1, x / 2, x / 3);

    const auto dst2 = sln::rgb_to_bgr(src1);
    REQUIRE(static_cast<std::uint64_t>(dst2[0]) == static_cast<std::uint64_t>(x / 3));
    REQUIRE(static_cast<std::uint64_t>(dst2[1]) == static_cast<std::uint64_t>(x / 2));
    REQUIRE(static_cast<std::uint64_t>(dst2[2]) == static_cast<std::uint64_t>(x / 1));

    const auto dst3 = sln::bgr_to_rgb(src1);
    REQUIRE(static_cast<std::uint64_t>(dst2[0]) == static_cast<std::uint64_t>(x / 3));
    REQUIRE(static_cast<std::uint64_t>(dst2[1]) == static_cast<std::uint64_t>(x / 2));
    REQUIRE(static_cast<std::uint64_t>(dst2[2]) == static_cast<std::uint64_t>(x / 1));
  }
}

template <typename T>
void check_random_pixel_conversions(T upper_bound = std::numeric_limits<T>::max(),
                                    T lower_bound = std::numeric_limits<T>::min())
{
  std::mt19937 rng(42);
  std::uniform_int_distribution<T> die(lower_bound, upper_bound);

  for (std::size_t i = 0; i < 1000; ++i)
  {
    const T x0 = die(rng);
    const T x1 = die(rng);
    const T x2 = die(rng);
    const T x3 = die(rng);

    sln::Pixel<T, 1> src_x(x0);
    sln::Pixel<T, 2> src_xx(x0, x1);
    sln::Pixel<T, 3> src_xxx(x0, x1, x2);
    sln::Pixel<T, 4> src_xxxx(x0, x1, x2, x3);

    REQUIRE(sln::y_to_ya(sln::ya_to_y(src_xx), x1) == src_xx);
    REQUIRE(sln::ya_to_y(sln::y_to_ya(src_x, x1)) == src_x);

    REQUIRE(sln::rgb_to_bgr(sln::bgr_to_rgb(src_xxx)) == src_xxx);
    REQUIRE(sln::rgb_to_rgba(sln::rgba_to_rgb(src_xxxx), x3) == src_xxxx);
    REQUIRE(sln::rgb_to_bgra(sln::bgra_to_rgb(src_xxxx), x3) == src_xxxx);
    REQUIRE(sln::rgb_to_argb(sln::argb_to_rgb(src_xxxx), x0) == src_xxxx);
    REQUIRE(sln::rgb_to_abgr(sln::abgr_to_rgb(src_xxxx), x0) == src_xxxx);

    REQUIRE(sln::bgr_to_rgba(sln::rgba_to_bgr(src_xxxx), x3) == src_xxxx);
    REQUIRE(sln::bgr_to_bgra(sln::bgra_to_bgr(src_xxxx), x3) == src_xxxx);
    REQUIRE(sln::bgr_to_argb(sln::argb_to_bgr(src_xxxx), x0) == src_xxxx);
    REQUIRE(sln::bgr_to_abgr(sln::abgr_to_bgr(src_xxxx), x0) == src_xxxx);

    REQUIRE(sln::rgba_to_bgra(sln::bgra_to_rgba(src_xxxx)) == src_xxxx);
    REQUIRE(sln::rgba_to_argb(sln::argb_to_rgba(src_xxxx)) == src_xxxx);
    REQUIRE(sln::rgba_to_abgr(sln::abgr_to_rgba(src_xxxx)) == src_xxxx);

    REQUIRE(sln::bgra_to_argb(sln::argb_to_bgra(src_xxxx)) == src_xxxx);
    REQUIRE(sln::bgra_to_abgr(sln::abgr_to_bgra(src_xxxx)) == src_xxxx);

    REQUIRE(sln::argb_to_abgr(sln::abgr_to_argb(src_xxxx)) == src_xxxx);
  }
}

TEST_CASE("Pixel conversions", "[img]")
{
  check_pixel_conversions<std::uint8_t>();
  check_pixel_conversions<std::int8_t>();

  check_pixel_conversions<std::uint16_t>();
  check_pixel_conversions<std::int16_t>();

  // Do not check the whole range, for efficiency reasons
  check_pixel_conversions<std::uint32_t>(sln::power(2u, 16u));
  check_pixel_conversions<std::int32_t>(sln::power(2, 15u), -sln::power(2, 15u));

  check_random_pixel_conversions<std::uint8_t>();
  check_random_pixel_conversions<std::int8_t>();
}

TEST_CASE("Image conversions", "[img]")
{
  const auto img_x = sln_test::make_3x3_test_image_8u1();
  const auto img_xxx = sln_test::make_3x3_test_image_8u3();

  const auto img_rgba = sln::transform_pixels<sln::Pixel_8u4>(
      img_x, [](const auto& px) { return sln::y_to_rgba(px, std::uint8_t{0}); });
  REQUIRE(img_rgba(0_px, 0_px) == sln::Pixel_8u4(10, 10, 10, 0));
  REQUIRE(img_rgba(1_px, 0_px) == sln::Pixel_8u4(20, 20, 20, 0));
  REQUIRE(img_rgba(2_px, 0_px) == sln::Pixel_8u4(30, 30, 30, 0));
  REQUIRE(img_rgba(0_px, 1_px) == sln::Pixel_8u4(40, 40, 40, 0));
  REQUIRE(img_rgba(1_px, 1_px) == sln::Pixel_8u4(50, 50, 50, 0));
  REQUIRE(img_rgba(2_px, 1_px) == sln::Pixel_8u4(60, 60, 60, 0));
  REQUIRE(img_rgba(0_px, 2_px) == sln::Pixel_8u4(70, 70, 70, 0));
  REQUIRE(img_rgba(1_px, 2_px) == sln::Pixel_8u4(80, 80, 80, 0));
  REQUIRE(img_rgba(2_px, 2_px) == sln::Pixel_8u4(90, 90, 90, 0));

  const auto img_bgr = sln::transform_pixels<sln::Pixel_8u3>(img_xxx,
                                                             [](const auto& px) { return sln::rgb_to_bgr(px); });
  REQUIRE(img_bgr(0_px, 0_px) == sln::Pixel_8u3(12, 11, 10));
  REQUIRE(img_bgr(1_px, 0_px) == sln::Pixel_8u3(22, 21, 20));
  REQUIRE(img_bgr(2_px, 0_px) == sln::Pixel_8u3(32, 31, 30));
  REQUIRE(img_bgr(0_px, 1_px) == sln::Pixel_8u3(42, 41, 40));
  REQUIRE(img_bgr(1_px, 1_px) == sln::Pixel_8u3(52, 51, 50));
  REQUIRE(img_bgr(2_px, 1_px) == sln::Pixel_8u3(62, 61, 60));
  REQUIRE(img_bgr(0_px, 2_px) == sln::Pixel_8u3(72, 71, 70));
  REQUIRE(img_bgr(1_px, 2_px) == sln::Pixel_8u3(82, 81, 80));
  REQUIRE(img_bgr(2_px, 2_px) == sln::Pixel_8u3(92, 91, 90));
}