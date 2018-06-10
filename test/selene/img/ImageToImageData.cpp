// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img/ImageTypeAliases.hpp>

using namespace sln::literals;

namespace {

template <typename PixelType>
struct PixelProducer;

template <typename T>
struct PixelProducer<sln::Pixel<T, 1>>
{
  static sln::Pixel<T, 1> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 1>(x + y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 2>>
{
  static sln::Pixel<T, 2> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 2>(x + y, 2 * x + y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 3>>
{
  static sln::Pixel<T, 3> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 3>(x + y, 2 * x + y, x + 2 * y);
  }
};

template <typename T>
struct PixelProducer<sln::Pixel<T, 4>>
{
  static sln::Pixel<T, 4> get(sln::PixelIndex x, sln::PixelIndex y)
  {
    return sln::Pixel<T, 4>(x + y, 2 * x + y, x + 2 * y, 2 * x + 2 * y);
  }
};

template <typename PixelType>
sln::Image<PixelType> create_test_image(sln::PixelLength width, sln::PixelLength height)
{
  sln::Image<PixelType> img(width, height);

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      img(x, y) = PixelProducer<PixelType>::get(x, y);
    }
  }

  return img;
}

template <typename PixelType>
void test_image(sln::PixelLength width, sln::PixelLength height)
{
  auto img = create_test_image<PixelType>(width, height);
  auto img_data_view = to_image_data_view(img, sln::PixelFormat::Unknown);
  REQUIRE(img_data_view.width() == width);
  REQUIRE(img_data_view.height() == height);
  REQUIRE(img_data_view.is_packed());
  REQUIRE(img_data_view.is_view());
  REQUIRE(img_data_view.is_valid());
  // TODO: Content tests
  // ...

  auto img_data = to_image_data(std::move(img), sln::PixelFormat::Unknown);
  REQUIRE(img_data.width() == width);
  REQUIRE(img_data.height() == height);
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(img_data.is_valid());
  // TODO: Content tests
  // ...
}

}  // namespace

TEST_CASE("Converting Image<> to ImageData", "[img]")
{
  {
    sln::Image_8u1 img;
    sln::ImageData<> img_data;
    REQUIRE_THROWS(img_data = sln::to_image_data(std::move(img), sln::PixelFormat::Unknown));
  }

  for (auto w = 1_px; w < 32_px; w += 1_px)
  {
    for (auto h = 1_px; h < 32_px; h += 1_px)
    {
      test_image<sln::Pixel_8u1>(w, h);
      test_image<sln::Pixel_8u2>(w, h);
      test_image<sln::Pixel_8u3>(w, h);
      test_image<sln::Pixel_8u4>(w, h);
    }
  }
}
