// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/img/interop/DynImageToImage.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <numeric>

using namespace sln::literals;

namespace {

sln::DynImage create_test_dyn_image(sln::UntypedLayout layout, sln::UntypedImageSemantics semantics)
{
  sln::DynImage dyn_img(layout, semantics);

  for (auto y = 0_idx; y < dyn_img.height(); ++y)
  {
    std::uint8_t* begin = dyn_img.byte_ptr(y);
    std::uint8_t* end = begin + dyn_img.row_bytes();
    std::iota(begin, end, static_cast<std::uint8_t>(y));
  }

  return dyn_img;
}

}  // namespace

TEST_CASE("Dynamic image to typed image", "[img]")
{
  SECTION("Packed image, single-channel")
  {
    auto dyn_img = create_test_dyn_image({16_px, 20_px, 1, 1, sln::Stride{16}},
                                         {sln::PixelFormat::Y, sln::SampleFormat::UnsignedInteger});

    SECTION("Dynamic image to view")
    {
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 16);
      REQUIRE(img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic image view to view")
    {
      const auto dyn_img_view = dyn_img.view();
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 16);
      REQUIRE(img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic constant image view to constant view")
    {
      const auto dyn_img_view = dyn_img.constant_view();
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 16);
      REQUIRE(img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic image to image")
    {
      auto img = sln::to_image<sln::Pixel_8u1>(std::move(dyn_img));
      REQUIRE(img.width() == 16);
      REQUIRE(img.height() == 20);
      REQUIRE(img.stride_bytes() == 16);
      REQUIRE(img.is_packed());
      REQUIRE(img.is_valid());
      for (auto y = 0_idx; y < img.height(); ++y)
      {
        for (auto x = 0_idx; x < img.width(); ++x)
        {
          REQUIRE(img(x, y) == x + y);
        }
      }
    }
  }

  SECTION("Non-packed image, single-channel")
  {
    auto dyn_img = create_test_dyn_image({16_px, 20_px, 1, 1, sln::Stride{19}},
                                         {sln::PixelFormat::Y, sln::SampleFormat::UnsignedInteger});

    SECTION("Dynamic image to view")
    {
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 19);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic image view to view")
    {
      const auto dyn_img_view = dyn_img.view();
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 19);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic constant image view to constant view")
    {
      const auto dyn_img_view = dyn_img.constant_view();
      auto img_view = sln::to_image_view<sln::Pixel_8u1>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 19);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == x + y);
        }
      }
    }

    SECTION("Dynamic image to image")
    {
      auto img = sln::to_image<sln::Pixel_8u1>(std::move(dyn_img));
      REQUIRE(img.width() == 16);
      REQUIRE(img.height() == 20);
      REQUIRE(img.stride_bytes() == 19);
      REQUIRE(!img.is_packed());
      REQUIRE(img.is_valid());
      for (auto y = 0_idx; y < img.height(); ++y)
      {
        for (auto x = 0_idx; x < img.width(); ++x)
        {
          REQUIRE(img(x, y) == x + y);
        }
      }
    }
  }

  SECTION("Non-packed image, multi-channel")
  {
    auto dyn_img = create_test_dyn_image({16_px, 20_px, 3, 1, sln::Stride{52}},
                                         {sln::PixelFormat::RGB, sln::SampleFormat::UnsignedInteger});

    SECTION("Dynamic image to view")
    {
      auto img_view = sln::to_image_view<sln::Pixel_8u3>(dyn_img);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 52);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == sln::Pixel_8u3(3 * x + 0 + y, 3 * x + 1 + y, 3 * x + 2 + y));
        }
      }
    }

    SECTION("Dynamic image view to view")
    {
      const auto dyn_img_view = dyn_img.view();
      auto img_view = sln::to_image_view<sln::Pixel_8u3>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 52);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == sln::Pixel_8u3(3 * x + 0 + y, 3 * x + 1 + y, 3 * x + 2 + y));
        }
      }
    }

    SECTION("Dynamic constant image view to constant view")
    {
      const auto dyn_img_view = dyn_img.constant_view();
      auto img_view = sln::to_image_view<sln::Pixel_8u3>(dyn_img_view);
      REQUIRE(img_view.width() == 16);
      REQUIRE(img_view.height() == 20);
      REQUIRE(img_view.stride_bytes() == 52);
      REQUIRE(!img_view.is_packed());
      REQUIRE(img_view.is_valid());
      for (auto y = 0_idx; y < img_view.height(); ++y)
      {
        for (auto x = 0_idx; x < img_view.width(); ++x)
        {
          REQUIRE(img_view(x, y) == sln::Pixel_8u3(3 * x + 0 + y, 3 * x + 1 + y, 3 * x + 2 + y));
        }
      }
    }

    SECTION("Dynamic image to image")
    {
      auto img = sln::to_image<sln::Pixel_8u3>(std::move(dyn_img));
      REQUIRE(img.width() == 16);
      REQUIRE(img.height() == 20);
      REQUIRE(img.stride_bytes() == 52);
      REQUIRE(!img.is_packed());
      REQUIRE(img.is_valid());
      for (auto y = 0_idx; y < img.height(); ++y)
      {
        for (auto x = 0_idx; x < img.width(); ++x)
        {
          REQUIRE(img(x, y) == sln::Pixel_8u3(3 * x + 0 + y, 3 * x + 1 + y, 3 * x + 2 + y));
        }
      }
    }
  }
}
