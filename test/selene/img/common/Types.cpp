// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

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
  REQUIRE(sln::impl::compute_stride_bytes(128, 16) == sln::Stride{128});

  REQUIRE(sln::impl::compute_stride_bytes(129, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(130, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(131, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(132, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(133, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(134, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(135, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(136, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(137, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(138, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(139, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(140, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(141, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(142, 16) == sln::Stride{128 + 16});
  REQUIRE(sln::impl::compute_stride_bytes(143, 16) == sln::Stride{128 + 16});

  REQUIRE(sln::impl::compute_stride_bytes(144, 16) == sln::Stride{144});
  REQUIRE(sln::impl::compute_stride_bytes(145, 16) == sln::Stride{144 + 16});

  REQUIRE(sln::impl::compute_stride_bytes(129, 0) == sln::Stride{129});
}