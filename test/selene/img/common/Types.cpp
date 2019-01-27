// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/common/Types.hpp>

TEST_CASE("Pixel index/length conversion", "[img]")
{
  REQUIRE(sln::to_pixel_index(10) == sln::PixelIndex{10});
  REQUIRE(sln::to_pixel_index(-10) == sln::PixelIndex{-10});

  REQUIRE(sln::to_pixel_length(10) == sln::PixelLength{10});
  REQUIRE(sln::to_pixel_length(-10) == sln::PixelLength{-10});

  using namespace sln::literals;

  REQUIRE(10_idx == sln::PixelIndex{10});
  REQUIRE(-10_idx == sln::PixelIndex{-10});

  REQUIRE(10_px == sln::PixelLength{10});
  REQUIRE(-10_px == sln::PixelLength{-10});
}

TEST_CASE("Stride computation", "[img]")
{
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{128}, sln::ImageRowAlignment{16}) == sln::Stride{128});

  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{129}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{130}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{131}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{132}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{133}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{134}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{135}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{136}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{137}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{138}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{139}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{140}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{141}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{142}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{143}, sln::ImageRowAlignment{16}) == sln::Stride{128 + 16});

  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{144}, sln::ImageRowAlignment{16}) == sln::Stride{144});
  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{145}, sln::ImageRowAlignment{16}) == sln::Stride{144 + 16});

  REQUIRE(sln::impl::compute_stride_bytes(sln::Stride{129}, sln::ImageRowAlignment{0}) == sln::Stride{129});
}