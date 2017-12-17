// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include "_TestImages.hpp"

using namespace sln::literals;


namespace sln_test {

sln::Image_8u1 make_3x3_test_image_8u1()
{
  sln::Image_8u1 img(3_px, 3_px);

  img(0_px, 0_px) = 10;
  img(1_px, 0_px) = 20;
  img(2_px, 0_px) = 30;

  img(0_px, 1_px) = 40;
  img(1_px, 1_px) = 50;
  img(2_px, 1_px) = 60;

  img(0_px, 2_px) = 70;
  img(1_px, 2_px) = 80;
  img(2_px, 2_px) = 90;

  return img;
}

sln::Image_8u3 make_3x3_test_image_8u3()
{
  sln::Image_8u3 img(3_px, 3_px);

  img(0_px, 0_px) = sln::Pixel_8u3(10, 11, 12);
  img(1_px, 0_px) = sln::Pixel_8u3(20, 21, 22);
  img(2_px, 0_px) = sln::Pixel_8u3(30, 31, 32);

  img(0_px, 1_px) = sln::Pixel_8u3(40, 41, 42);
  img(1_px, 1_px) = sln::Pixel_8u3(50, 51, 52);
  img(2_px, 1_px) = sln::Pixel_8u3(60, 61, 62);

  img(0_px, 2_px) = sln::Pixel_8u3(70, 71, 72);
  img(1_px, 2_px) = sln::Pixel_8u3(80, 81, 82);
  img(2_px, 2_px) = sln::Pixel_8u3(90, 91, 92);

  return img;
}

}  // namespace sln_test
