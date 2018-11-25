// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/old_img/Image.hpp>
#include <selene/old_img/ImageData.hpp>
#include <selene/old_img/ImageDataToImage.hpp>
#include <selene/old_img/ImageTypeAliases.hpp>

#include <numeric>

using namespace sln::literals;

namespace {

sln::ImageData<> create_test_image_data(sln::PixelLength width,
                                        sln::PixelLength height,
                                        std::uint16_t nr_channels,
                                        std::uint8_t nr_bytes_per_channel,
                                        sln::Stride stride_bytes,
                                        sln::PixelFormat pixel_format,
                                        sln::SampleFormat sample_format)
{
  sln::ImageData<> img_data(width, height, nr_channels, nr_bytes_per_channel, stride_bytes, pixel_format,
                            sample_format);

  for (auto y = 0_idx; y < img_data.height(); ++y)
  {
    std::uint8_t* begin = img_data.byte_ptr(y);
    std::uint8_t* end = begin + img_data.row_bytes();
    std::iota(begin, end, static_cast<std::uint8_t>(y));
  }

  return img_data;
}

}  // namespace

TEST_CASE("Converting ImageData to Image<>", "[old_img]")
{
  // Packed image, single-channel
  {
    auto img_data = create_test_image_data(16_px, 20_px, 1, 1, sln::Stride{16}, sln::PixelFormat::Y,
                                           sln::SampleFormat::UnsignedInteger);

    auto img_view = sln::to_image_view<sln::Pixel_8u1>(img_data);
    REQUIRE(img_view.width() == 16);
    REQUIRE(img_view.height() == 20);
    REQUIRE(img_view.stride_bytes() == 16);
    REQUIRE(img_view.is_packed());
    REQUIRE(img_view.is_view());
    REQUIRE(img_view.is_valid());
    for (auto y = 0_idx; y < img_view.height(); ++y)
    {
      for (auto x = 0_idx; x < img_view.width(); ++x)
      {
        REQUIRE(img_view(x, y) == x + y);
      }
    }

    auto img = sln::to_image<sln::Pixel_8u1>(std::move(img_data));
    REQUIRE(img.width() == 16);
    REQUIRE(img.height() == 20);
    REQUIRE(img.stride_bytes() == 16);
    REQUIRE(img.is_packed());
    REQUIRE(!img.is_view());
    REQUIRE(img.is_valid());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == x + y);
      }
    }
  }

  // Non-packed image, single-channel
  {
    auto img_data = create_test_image_data(16_px, 20_px, 1, 1, sln::Stride{19}, sln::PixelFormat::Y,
                                           sln::SampleFormat::UnsignedInteger);

    auto img_view = sln::to_image_view<sln::Pixel_8u1>(img_data);
    REQUIRE(img_view.width() == 16);
    REQUIRE(img_view.height() == 20);
    REQUIRE(img_view.stride_bytes() == 19);
    REQUIRE(!img_view.is_packed());
    REQUIRE(img_view.is_view());
    REQUIRE(img_view.is_valid());
    for (auto y = 0_idx; y < img_view.height(); ++y)
    {
      for (auto x = 0_idx; x < img_view.width(); ++x)
      {
        REQUIRE(img_view(x, y) == x + y);
      }
    }

    auto img = sln::to_image<sln::Pixel_8u1>(std::move(img_data));
    REQUIRE(img.width() == 16);
    REQUIRE(img.height() == 20);
    REQUIRE(img.stride_bytes() == 19);
    REQUIRE(!img.is_packed());
    REQUIRE(!img.is_view());
    REQUIRE(img.is_valid());
    for (auto y = 0_idx; y < img.height(); ++y)
    {
      for (auto x = 0_idx; x < img.width(); ++x)
      {
        REQUIRE(img(x, y) == x + y);
      }
    }
  }

  // Non-packed image, multi-channel
  {
    auto img_data = create_test_image_data(16_px, 20_px, 3, 1, sln::Stride{52}, sln::PixelFormat::RGB,
                                           sln::SampleFormat::UnsignedInteger);

    auto img_view = sln::to_image_view<sln::Pixel_8u3>(img_data);
    REQUIRE(img_view.width() == 16);
    REQUIRE(img_view.height() == 20);
    REQUIRE(img_view.stride_bytes() == 52);
    REQUIRE(!img_view.is_packed());
    REQUIRE(img_view.is_view());
    REQUIRE(img_view.is_valid());
    for (auto y = 0_idx; y < img_view.height(); ++y)
    {
      for (auto x = 0_idx; x < img_view.width(); ++x)
      {
        REQUIRE(img_view(x, y) == sln::Pixel_8u3(3 * x + 0 + y, 3 * x + 1 + y, 3 * x + 2 + y));
      }
    }

    auto img = sln::to_image<sln::Pixel_8u3>(std::move(img_data));
    REQUIRE(img.width() == 16);
    REQUIRE(img.height() == 20);
    REQUIRE(img.stride_bytes() == 52);
    REQUIRE(!img.is_packed());
    REQUIRE(!img.is_view());
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
