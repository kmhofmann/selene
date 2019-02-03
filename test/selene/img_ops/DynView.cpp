// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img_ops/DynView.hpp>

#include <array>
#include <cstdint>

using namespace sln::literals;

namespace {

std::array<std::uint8_t, 16> dyn_img_arr_44 = {{
   10,  20,  30,  40,
   50,  60,  70,  80,
   90, 100, 110, 120,
  130, 140, 150, 160 }};

sln::DynImage<> create_dyn_image()
{
  sln::DynImage<> dyn_img{sln::UntypedLayout{4_px, 4_px, 1, 1},
                          sln::UntypedImageSemantics{}};
  std::copy(dyn_img_arr_44.cbegin(), dyn_img_arr_44.cend(), dyn_img.byte_ptr());
  return dyn_img;
}

template <sln::ImageModifiability modifiability>
sln::DynImageView<modifiability> create_dyn_image_view()
{
  return sln::DynImageView<modifiability>{dyn_img_arr_44.data(),
                                          sln::UntypedLayout{4_px, 4_px, 1, 1},
                                          sln::UntypedImageSemantics{}};
}

template <typename PixelType, typename T>
void check_result(sln::PixelIndex x_beg, sln::PixelIndex y_beg,
                  sln::PixelIndex x_end, sln::PixelIndex y_end,
                  const T& img)
{
  for (auto y = y_beg; y < y_end; ++y)
  {
    for (auto x = x_beg; x < x_end; ++x)
    {
      const auto idx = std::size_t(y * 4 + x);
      REQUIRE(img.template pixel<PixelType>(x - x_beg, y - y_beg) == PixelType{dyn_img_arr_44[idx]});
    }
  }
}

}  // namespace _

TEST_CASE("Dynamic image view creation", "[img]")
{
  using PixelType = sln::Pixel_8u1;
  const auto bbox = sln::BoundingBox{1_idx, 1_idx, 2_px, 2_px};

  SECTION("Constant view")
  {
    auto constant_dyn_img_view = create_dyn_image_view<sln::ImageModifiability::Constant>();
    auto v = sln::view(constant_dyn_img_view);
    REQUIRE(v.width() == constant_dyn_img_view.width());
    REQUIRE(v.height() == constant_dyn_img_view.height());
    check_result<PixelType>(sln::PixelIndex{0}, sln::PixelIndex{0},
                            sln::PixelIndex{v.width()}, sln::PixelIndex{v.height()}, v);
  }

  SECTION("Mutable view")
  {
    auto mutable_dyn_img_view = create_dyn_image_view<sln::ImageModifiability::Mutable>();
    auto v = sln::view(mutable_dyn_img_view);
    REQUIRE(v.width() == mutable_dyn_img_view.width());
    REQUIRE(v.height() == mutable_dyn_img_view.height());
    check_result<PixelType>(sln::PixelIndex{0}, sln::PixelIndex{0},
                            sln::PixelIndex{v.width()}, sln::PixelIndex{v.height()}, v);
  }

  SECTION("Constant image")
  {
    const auto const_dyn_img = create_dyn_image();
    auto v = sln::view(const_dyn_img);
    REQUIRE(v.width() == const_dyn_img.width());
    REQUIRE(v.height() == const_dyn_img.height());
    check_result<PixelType>(sln::PixelIndex{0}, sln::PixelIndex{0},
                            sln::PixelIndex{v.width()}, sln::PixelIndex{v.height()}, v);
  }

  SECTION("Mutable image")
  {
    auto dyn_img = create_dyn_image();
    auto v = sln::view(dyn_img);
    REQUIRE(v.width() == dyn_img.width());
    REQUIRE(v.height() == dyn_img.height());
    check_result<PixelType>(sln::PixelIndex{0}, sln::PixelIndex{0},
                            sln::PixelIndex{v.width()}, sln::PixelIndex{v.height()}, v);
  }

  SECTION("Constant view, with region")
  {
    auto constant_dyn_img_view = create_dyn_image_view<sln::ImageModifiability::Constant>();
    auto v = sln::view<PixelType>(constant_dyn_img_view, bbox);
    REQUIRE(v.width() == bbox.width());
    REQUIRE(v.height() == bbox.height());
    check_result<PixelType>(bbox.x0(), bbox.y0(), bbox.x1(), bbox.y1(), v);
  }

  SECTION("Mutable view, with region")
  {
    auto mutable_dyn_img_view = create_dyn_image_view<sln::ImageModifiability::Mutable>();
    auto v = sln::view<PixelType>(mutable_dyn_img_view, bbox);
    REQUIRE(v.width() == bbox.width());
    REQUIRE(v.height() == bbox.height());
    check_result<PixelType>(bbox.x0(), bbox.y0(), bbox.x1(), bbox.y1(), v);
  }

  SECTION("Constant image, with region")
  {
    const auto const_dyn_img = create_dyn_image();
    auto v = sln::view<PixelType>(const_dyn_img, bbox);
    REQUIRE(v.width() == bbox.width());
    REQUIRE(v.height() == bbox.height());
    check_result<PixelType>(bbox.x0(), bbox.y0(), bbox.x1(), bbox.y1(), v);
  }

  SECTION("Mutable image, with region")
  {
    auto dyn_img = create_dyn_image();
    auto v = sln::view<PixelType>(dyn_img, bbox);
    REQUIRE(v.width() == bbox.width());
    REQUIRE(v.height() == bbox.height());
    check_result<PixelType>(bbox.x0(), bbox.y0(), bbox.x1(), bbox.y1(), v);
  }


}