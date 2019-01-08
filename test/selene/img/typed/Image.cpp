// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img_ops/Fill.hpp>

//#include <test/selene/img/typed/Utils.hpp>

using namespace sln::literals;

namespace {

template <typename T>
void basic_image_tests(sln::PixelLength width, sln::PixelLength height, T fill_value)
{
  {
    sln::Image<T> img;
  }

  sln::Image<T> img0({width, height});
  REQUIRE(img0.width() == width);
  REQUIRE(img0.height() == height);
  REQUIRE(img0.stride_bytes() == img0.width() * static_cast<std::ptrdiff_t>(sizeof(T)));
  REQUIRE(img0.is_packed());
  REQUIRE(!img0.is_empty());

  sln::fill(img0, fill_value);
  for (auto y = 0_idx; y < img0.height(); ++y)
  {
    auto ptr = img0.data(y);
    for (auto x = 0_idx; x < img0.width(); ++x)
    {
      REQUIRE(img0(x, y) == fill_value);
      REQUIRE(*(ptr + x) == fill_value);
    }
  }

  sln::Image<T> img1 = img0;
  REQUIRE(img1.width() == img0.width());
  REQUIRE(img1.height() == img0.height());
  REQUIRE(img1.stride_bytes() == img0.stride_bytes());
  REQUIRE(img1.is_packed());
  REQUIRE(!img1.is_empty());

  for (std::ptrdiff_t alignment = 1; alignment <= 128; alignment <<= 1)
  {
    sln::Image<T> img({width, height}, sln::ImageRowAlignment{alignment});
    REQUIRE(img.width() == width);
    REQUIRE(img.height() == height);
    REQUIRE(img.stride_bytes() % alignment == 0);
    REQUIRE(!img.is_empty());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      REQUIRE(reinterpret_cast<std::uintptr_t>(img.data(y)) % std::uintptr_t(alignment) == 0);
    }

    const auto alignment2 = alignment / 2;
    img.reallocate({sln::PixelLength{width + 1}, sln::PixelLength{height + 1}}, sln::ImageRowAlignment{alignment2});
    REQUIRE(img.width() == width + 1);
    REQUIRE(img.height() == height + 1);
    REQUIRE((alignment2 == 0 || img.stride_bytes() % alignment2 == 0));
    REQUIRE(!img.is_empty());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      REQUIRE((alignment2 == 0 || reinterpret_cast<std::uintptr_t>(img.data(y)) % std::uintptr_t(alignment2) == 0));
    }
  }
}

}  // namespace


TEST_CASE("Image construction", "[img]")
{
  constexpr auto fill_value = 42;
  for (auto h = 10_px; h < 100_px; h += 20_px)
  {
    for (auto w = 10_px; w < 100_px; w += 10_px)
    {
      basic_image_tests<std::uint8_t>(w, h, fill_value);
      basic_image_tests<std::int8_t>(w, h, fill_value);
      basic_image_tests<std::uint16_t>(w, h, fill_value);
      basic_image_tests<std::int16_t>(w, h, fill_value);
      basic_image_tests<std::uint32_t>(w, h, fill_value);
      basic_image_tests<std::int32_t>(w, h, fill_value);
      basic_image_tests<std::uint64_t>(w, h, fill_value);
      basic_image_tests<std::int64_t>(w, h, fill_value);
      basic_image_tests<sln::float32_t>(w, h, sln::float32_t{fill_value});
      basic_image_tests<sln::float64_t>(w, h, sln::float64_t{fill_value});
    }
  }
}
