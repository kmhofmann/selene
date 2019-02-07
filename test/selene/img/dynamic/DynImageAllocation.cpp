// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/dynamic/DynImage.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <test/selene/Utils.hpp>

using namespace sln::literals;


TEST_CASE("DynImage allocation", "[img]")
{
  using DynImage = sln::DynImage<sln_test::IotaAllocator<std::uint8_t>>;
  std::uint8_t value{42};
  DynImage dyn_img({10_px, 20_px, 1, 1},
                   {sln::PixelFormat::Y, sln::SampleFormat::UnsignedInteger},
                   sln_test::IotaAllocator<std::uint8_t>{value});

  REQUIRE(dyn_img.is_valid());
  REQUIRE(dyn_img.width() == 10_px);
  REQUIRE(dyn_img.height() == 20_px);
  REQUIRE(dyn_img.nr_channels() == 1);
  REQUIRE(dyn_img.nr_bytes_per_channel() == 1);

  for (auto row_it = dyn_img.begin<sln::PixelY_8u>(), row_it_end = dyn_img.end<sln::PixelY_8u>(); row_it != row_it_end; ++row_it)
  {
    for (const auto& px : *row_it)
    {
      REQUIRE(px == value++);
    }
  }
}

