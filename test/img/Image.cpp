// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/img/Image.hpp>

using namespace selene;
using namespace selene::img;

template <typename T>
void image_tests(Index width, Index height, T fill_value)
{
  Image<T> img(width, height);
  REQUIRE((img.width() == width));
  REQUIRE((img.height() == height));
  REQUIRE(img.stride_bytes() == img.width() * sizeof(T));
  REQUIRE(img.is_packed());
  REQUIRE(!img.is_view());
  REQUIRE(!img.is_empty());

  img.fill(fill_value);
  for (Index y = 0_px; y < img.height(); ++y)
  {
    auto ptr = img.data(y);
    for (Index x = 0_px; x < img.width(); ++x)
    {
      REQUIRE(img(x, y) == fill_value);
      REQUIRE(*(ptr + x) == fill_value);
    }
  }

  Image<T> img2;
  clone(img, img2);
  REQUIRE(img2.width() == img.width());
  REQUIRE(img2.height() == img.height());
  REQUIRE(img2.stride_bytes() == img.stride_bytes());
}

TEST_CASE("Image construction", "[img]")
{
  for (Index h = 10_px; h < 100_px; h += 20_px)
  {
    for (Index w = 10_px; w < 100_px; w += 10_px)
    {
      image_tests<unsigned char>(w, h, 42);
    }
  }
}
