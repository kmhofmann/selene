// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/common/RowPointers.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/typed/Image.hpp>

using namespace sln::literals;

TEST_CASE("Row pointers / typed image", "[img]")
{
  using namespace sln::literals;
  using PixelType = sln::Pixel<std::uint8_t, 1>;

  sln::Image<PixelType> img({20_px, 10_px});
  sln::MutableImageView<PixelType> view(img.byte_ptr(), img.layout());
  sln::ConstantImageView<PixelType> cview(img.byte_ptr(), img.layout());

  const auto row_ptrs_img = sln::get_row_pointers(img);
  const auto row_ptrs_view = sln::get_row_pointers(view);
  const auto row_ptrs_cview = sln::get_row_pointers(cview);

  REQUIRE(row_ptrs_img.size() == row_ptrs_view.size());

  for (std::size_t i = 0; i < row_ptrs_img.size(); ++i)
  {
    REQUIRE(row_ptrs_img[i] == row_ptrs_view[i]);
  }
}

TEST_CASE("Row pointers / dynamic image", "[img]")
{
  using namespace sln::literals;

  sln::DynImage img({20_px, 10_px, 1, 2});
  sln::MutableDynImageView view(img.byte_ptr(), img.layout());
  sln::ConstantDynImageView cview(img.byte_ptr(), img.layout());

  const auto row_ptrs_img = sln::get_row_pointers(img);
  const auto row_ptrs_view = sln::get_row_pointers(view);
  const auto row_ptrs_cview = sln::get_row_pointers(cview);

  REQUIRE(row_ptrs_img.size() == row_ptrs_view.size());

  for (std::size_t i = 0; i < row_ptrs_img.size(); ++i)
  {
    REQUIRE(row_ptrs_img[i] == row_ptrs_view[i]);
  }
}