// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img_ops/PixelConversions.hpp>

#include <selene/base/Utils.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img_ops/Algorithms.hpp>

#include <limits>
#include <random>
#include <type_traits>

#include <test/utils/Utils.hpp>
#include <test/selene/img/typed/_Utils.hpp>

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

    const auto dst_rgb_to_y = sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::Y>(src_xxx);
    REQUIRE(static_cast<std::uint64_t>(dst_rgb_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_bgr_to_y = sln::convert_pixel<sln::PixelFormat::BGR, sln::PixelFormat::Y>(src_xxx);
    REQUIRE(static_cast<std::uint64_t>(dst_bgr_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_rgba_to_y = sln::convert_pixel<sln::PixelFormat::RGBA, sln::PixelFormat::Y>(src_xxxa);
    REQUIRE(static_cast<std::uint64_t>(dst_rgba_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_bgra_to_y = sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::Y>(src_xxxa);
    REQUIRE(static_cast<std::uint64_t>(dst_bgra_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_argb_to_y = sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::Y>(src_axxx);
    REQUIRE(static_cast<std::uint64_t>(dst_argb_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_abgr_to_y = sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::Y>(src_axxx);
    REQUIRE(static_cast<std::uint64_t>(dst_abgr_to_y) == static_cast<std::uint64_t>(x));

    const auto dst_rgb_to_ya = sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::YA>(src_xxx, T{0});
    REQUIRE(dst_rgb_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    const auto dst_bgr_to_ya = sln::convert_pixel<sln::PixelFormat::BGR, sln::PixelFormat::YA>(src_xxx, T{0});
    REQUIRE(dst_bgr_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    const auto dst_rgba_to_ya = sln::convert_pixel<sln::PixelFormat::RGBA, sln::PixelFormat::YA>(src_xxxa);
    REQUIRE(dst_rgba_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    const auto dst_bgra_to_ya = sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::YA>(src_xxxa);
    REQUIRE(dst_bgra_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    const auto dst_argb_to_ya = sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::YA>(src_axxx);
    REQUIRE(dst_argb_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    const auto dst_abgr_to_ya = sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::YA>(src_axxx);
    REQUIRE(dst_abgr_to_ya == sln::Pixel<T, 2, sln::PixelFormat::YA>(x, 0));

    sln::Pixel<T, 3> src1(x / 1, x / 2, x / 3);

    const auto dst2 = sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::BGR>(src1);
    REQUIRE(static_cast<std::uint64_t>(dst2[0]) == static_cast<std::uint64_t>(x / 3));
    REQUIRE(static_cast<std::uint64_t>(dst2[1]) == static_cast<std::uint64_t>(x / 2));
    REQUIRE(static_cast<std::uint64_t>(dst2[2]) == static_cast<std::uint64_t>(x / 1));

    const auto dst3 = sln::convert_pixel<sln::PixelFormat::BGR, sln::PixelFormat::RGB>(src1);
    REQUIRE(static_cast<std::uint64_t>(dst3[0]) == static_cast<std::uint64_t>(x / 3));
    REQUIRE(static_cast<std::uint64_t>(dst3[1]) == static_cast<std::uint64_t>(x / 2));
    REQUIRE(static_cast<std::uint64_t>(dst3[2]) == static_cast<std::uint64_t>(x / 1));
  }
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void floating_point_conversions(const sln::Pixel<T, 3>&, const sln::Pixel<T, 4>&)
{
}

template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>, typename = void>
void floating_point_conversions(const sln::Pixel<T, 3>& src_xxx, const sln::Pixel<T, 4>& src_xxxx)
{
  constexpr auto f_r = sln::impl::RGBToYCoefficients::values[0];
  constexpr auto f_g = sln::impl::RGBToYCoefficients::values[1];
  constexpr auto f_b = sln::impl::RGBToYCoefficients::values[2];

  const auto dst_rgb_to_y = sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::Y>(src_xxx);
  REQUIRE(static_cast<float>(dst_rgb_to_y)
          == Approx(f_r * src_xxx[0] + f_g * src_xxx[1] + f_b * src_xxx[2]).epsilon(0.001));

  const auto dst_rgba_to_y = sln::convert_pixel<sln::PixelFormat::RGBA, sln::PixelFormat::Y>(src_xxxx);
  REQUIRE(static_cast<float>(dst_rgba_to_y)
          == Approx(f_r * src_xxxx[0] + f_g * src_xxxx[1] + f_b * src_xxxx[2]).epsilon(0.001));

  const auto dst_bgr_to_y = sln::convert_pixel<sln::PixelFormat::BGR, sln::PixelFormat::Y>(src_xxx);
  REQUIRE(static_cast<float>(dst_bgr_to_y)
          == Approx(f_b * src_xxx[0] + f_g * src_xxx[1] + f_r * src_xxx[2]).epsilon(0.001));

  const auto dst_bgra_to_y = sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::Y>(src_xxxx);
  REQUIRE(static_cast<float>(dst_bgra_to_y)
          == Approx(f_b * src_xxxx[0] + f_g * src_xxxx[1] + f_r * src_xxxx[2]).epsilon(0.001));
}

template <typename T>
void check_random_pixel_conversions(T upper_bound = std::numeric_limits<T>::max(),
                                    T lower_bound = std::numeric_limits<T>::min())
{
  std::mt19937 rng(42);
  auto die = sln_test::uniform_distribution<T>(lower_bound, upper_bound);

  for (std::size_t i = 0; i < 1000; ++i)
  {
    const T x0 = static_cast<T>(die(rng));
    const T x1 = static_cast<T>(die(rng));
    const T x2 = static_cast<T>(die(rng));
    const T x3 = static_cast<T>(die(rng));

    sln::Pixel<T, 1> src_x(x0);
    sln::Pixel<T, 2> src_xx(x0, x1);
    sln::Pixel<T, 3> src_xxx(x0, x1, x2);
    sln::Pixel<T, 4> src_xxxx(x0, x1, x2, x3);

    // Check that back-and-forth conversions yield the input result again

    REQUIRE(sln::convert_pixel<sln::PixelFormat::YA>(
                sln::convert_pixel<sln::PixelFormat::YA, sln::PixelFormat::Y>(src_xx), x1)
            == src_xx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::Y>(
                sln::convert_pixel<sln::PixelFormat::Y, sln::PixelFormat::YA>(src_x, x1))
            == src_x);

    REQUIRE(sln::convert_pixel<sln::PixelFormat::BGR>(
                sln::convert_pixel<sln::PixelFormat::BGR, sln::PixelFormat::RGB>(src_xxx))
            == src_xxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::RGBA>(
                sln::convert_pixel<sln::PixelFormat::RGBA, sln::PixelFormat::RGB>(src_xxxx), x3)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::BGRA>(
                sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::RGB>(src_xxxx), x3)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ARGB>(
                sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::RGB>(src_xxxx), x0)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ABGR>(
                sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::RGB>(src_xxxx), x0)
            == src_xxxx);

    REQUIRE(sln::convert_pixel<sln::PixelFormat::RGBA>(
                sln::convert_pixel<sln::PixelFormat::RGBA, sln::PixelFormat::BGR>(src_xxxx), x3)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::BGRA>(
                sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::BGR>(src_xxxx), x3)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ARGB>(
                sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::BGR>(src_xxxx), x0)
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ABGR>(
                sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::BGR>(src_xxxx), x0)
            == src_xxxx);

    REQUIRE(sln::convert_pixel<sln::PixelFormat::BGRA>(
                sln::convert_pixel<sln::PixelFormat::BGRA, sln::PixelFormat::RGBA>(src_xxxx))
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ARGB>(
                sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::RGBA>(src_xxxx))
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ABGR>(
                sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::RGBA>(src_xxxx))
            == src_xxxx);

    REQUIRE(sln::convert_pixel<sln::PixelFormat::ARGB>(
                sln::convert_pixel<sln::PixelFormat::ARGB, sln::PixelFormat::BGRA>(src_xxxx))
            == src_xxxx);
    REQUIRE(sln::convert_pixel<sln::PixelFormat::ABGR>(
                sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::BGRA>(src_xxxx))
            == src_xxxx);

    REQUIRE(sln::convert_pixel<sln::PixelFormat::ABGR>(
                sln::convert_pixel<sln::PixelFormat::ABGR, sln::PixelFormat::ARGB>(src_xxxx))
            == src_xxxx);

    floating_point_conversions(src_xxx, src_xxxx);
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

  check_random_pixel_conversions<std::uint16_t>();
  check_random_pixel_conversions<std::int16_t>();

  check_random_pixel_conversions<std::uint32_t>();
  check_random_pixel_conversions<std::int32_t>();

  check_random_pixel_conversions<std::uint64_t>();
  check_random_pixel_conversions<std::int64_t>();

  check_random_pixel_conversions<sln::float32_t>(10000.0f, -10000.0f);
  check_random_pixel_conversions<sln::float64_t>(10000.0f, -10000.0f);

  constexpr auto px = sln::convert_pixel<sln::PixelFormat::RGB, sln::PixelFormat::Y>(sln::Pixel_8u3(100, 100, 100));
  REQUIRE(px == 100);
}
