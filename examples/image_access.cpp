// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/ImageAccess.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img_io/IO.hpp>

#include <selene/io/FileWriter.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_interpolated = "bike_duck_bilinear_interpolation.png";
constexpr auto output_filename_border_access = "bike_duck_border_access.png";

using namespace sln;  // Never outside of example code

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `Pixel_8u3` designates 3 channels of unsigned 8-bit data for each pixel.

  auto img = sln_examples::read_example_image<Pixel_8u3>("bike_duck.png", data_path);
  assert(img.width() == 1024);
  assert(img.height() == 684);

  const auto target_width = 300_px;
  const auto target_height = 150_px;

  std::cout << "Sampling image pixels with interpolation and border policy...\n";

  // We can now sample pixels in the read image by means of bilinear interpolation.
  // - The function get() defaults to bilinear interpolation when given floating point coordinates.
  // - Alternatively, we could explicitly specify an interpolation policy, or additionally a border access policy:
  //   - get<ImageInterpolationMode::NearestNeighbor>(img, fx, fy)
  //   - get<ImageInterpolationMode::Bilinear, BorderAccessMode::ZeroPadding>(img, fx, fy)

  Image<Pixel_8u3> img_interpolation(target_width, target_height);

  for (auto y = 0_px; y < target_height; ++y)
  {
    for (auto x = 0_px; x < target_width; ++x)
    {
      const double fx = 130.0 + 0.83 * x;
      const double fy = 100.0 + 1.46 * y;
      img_interpolation(x, y) = get(img, fx, fy);
    }
  }

  // When given integer coordinates, get() performs no interpolation, and is by default equivalent to the Image<>
  // operator().
  // - We can, however, explicitly specify a border access mode, such as:
  //   - get<BorderAccessMode::Unchecked>(img, x, y);
  //   - get<BorderAccessMode::Replicated>(img, x, y);
  //   - get<BorderAccessMode::ZeroPadding>(img, x, y);

  Image<Pixel_8u3> img_border_access(img.width(), img.height());

  for (auto y = 0_px; y < img.height(); ++y)
  {
    for (auto x = 0_px; x < img.width(); ++x)
    {
      img_border_access(x, y) = get<BorderAccessMode::Replicated>(img, PixelIndex{x + 100}, PixelIndex{y - 200});
    }
  }

  std::cout << "Writing the result to disk: '" << output_filename_interpolated << "'...\n";
  write_image(to_image_data_view(img_interpolation, PixelFormat::RGB), ImageFormat::PNG,
              FileWriter(output_filename_interpolated));

  std::cout << "Writing the result to disk: '" << output_filename_border_access << "'...\n";
  write_image(to_image_data_view(img_border_access, PixelFormat::RGB), ImageFormat::PNG,
              FileWriter(output_filename_border_access));

  return 0;
}
