// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/typed/Image.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <test/utils/Utils.hpp>

using namespace sln::literals;


TEST_CASE("Image allocation", "[img]")
{
  using Image = sln::Image<sln::PixelY_8u, sln_test::IotaAllocator<std::uint8_t>>;
  std::uint8_t value{42};
  Image img({10_px, 20_px}, sln_test::IotaAllocator<std::uint8_t>{value});

  REQUIRE(img.is_valid());
  REQUIRE(img.width() == 10_px);
  REQUIRE(img.height() == 20_px);

  for (const auto& row : img)
  {
    for (const auto& px : row)
    {
      REQUIRE(px == value++);
    }
  }
}
