// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/ImageToImageData.hpp>

using namespace selene;
using namespace selene::img;

namespace {

template <typename PixelType> struct PixelProducer;

template <typename T> struct PixelProducer<Pixel<T, 1>>
{
  static Pixel<T, 1> get(Index x, Index y) { return Pixel<T, 1>(x + y); }
};

template <typename T> struct PixelProducer<Pixel<T, 2>>
{
  static Pixel<T, 2> get(Index x, Index y) { return Pixel<T, 2>(x + y, 2 * x + y); }
};

template <typename T> struct PixelProducer<Pixel<T, 3>>
{
  static Pixel<T, 3> get(Index x, Index y) { return Pixel<T, 3>(x + y, 2 * x + y, x + 2 * y); }
};

template <typename T> struct PixelProducer<Pixel<T, 4>>
{
  static Pixel<T, 4> get(Index x, Index y) { return Pixel<T, 4>(x + y, 2 * x + y, x + 2 * y, 2 * x + 2 * y); }
};

template <typename PixelType> Image<PixelType> create_test_image(Length width, Length height)
{
  Image<PixelType> img(width, height);

  for (Index y = 0; y < img.height(); ++y)
  {
    for (Index x = 0; x < img.width(); ++x)
    {
      img(x, y) = PixelProducer<PixelType>::get(x, y);
    }
  }

  return img;
}

template <typename PixelType> void test_image(Length width, Length height)
{
  auto img = create_test_image<PixelType>(width, height);
  auto img_data_view = to_image_data_view(img, PixelFormat::Unknown);
  REQUIRE(img_data_view.width() == width);
  REQUIRE(img_data_view.height() == height);
  REQUIRE(img_data_view.is_packed());
  REQUIRE(img_data_view.is_view());
  REQUIRE(img_data_view.is_valid());
  // TODO: Content tests
  // ...

  auto img_data = to_image_data(std::move(img), PixelFormat::Unknown);
  REQUIRE(img_data.width() == width);
  REQUIRE(img_data.height() == height);
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(img_data.is_valid());
  // TODO: Content tests
  // ...
}

} // namespace _

TEST_CASE("Converting Image<> to ImageData", "[img]")
{
  {
    Image<Pixel_8u1> img;
    ImageData img_data;
    REQUIRE_THROWS(img_data = to_image_data(std::move(img), PixelFormat::Unknown));
  }

  for (Length w = 1; w < 32; w += 1)
  {
    for (Length h = 1; h < 32; h += 1)
    {
      test_image<Pixel_8u1>(w, h);
      test_image<Pixel_8u2>(w, h);
      test_image<Pixel_8u3>(w, h);
      test_image<Pixel_8u4>(w, h);
    }
  }
}
