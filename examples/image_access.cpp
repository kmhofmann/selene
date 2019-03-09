// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/access/GetPixel.hpp>

#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/IO.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_interpolated = "bike_duck_bilinear_interpolation.png";
constexpr auto output_filename_border_access = "bike_duck_border_access.png";

using namespace sln::literals;

int main(int argc, char* argv[])
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `Pixel_8u3` designates 3 channels of unsigned 8-bit data for each pixel.

  auto img = sln_examples::read_example_image<sln::PixelRGB_8u>("bike_duck.png", data_path);
  assert(img.width() == 1024_px);
  assert(img.height() == 684_px);

  const auto target_width = 300_px;
  const auto target_height = 150_px;

  std::cout << "Sampling image pixels with interpolation and border policy...\n";

  // We can now sample pixels in the read image by means of bilinear interpolation.
  // - The function get() defaults to bilinear interpolation when given floating point coordinates.
  // - Alternatively, we could explicitly specify an interpolation policy, or additionally a border access policy:
  //   - get<ImageInterpolationMode::NearestNeighbor>(img, fx, fy)
  //   - get<ImageInterpolationMode::Bilinear, BorderAccessMode::ZeroPadding>(img, fx, fy)

  sln::Image<sln::PixelRGB_8u> img_interpolation({target_width, target_height});

  for (auto y = 0_idx; y < target_height; ++y)
  {
    for (auto x = 0_idx; x < target_width; ++x)
    {
      const double fx = double{130.0 + 0.83 * x};
      const double fy = double{100.0 + 1.46 * y};
      img_interpolation(x, y) = get(img, fx, fy);
    }
  }

  // When given integer coordinates, get() performs no interpolation, and is by default equivalent to the Image<>
  // operator().
  // - We can, however, explicitly specify a border access mode, such as:
  //   - get<BorderAccessMode::Unchecked>(img, x, y);
  //   - get<BorderAccessMode::Replicated>(img, x, y);
  //   - get<BorderAccessMode::ZeroPadding>(img, x, y);

  sln::Image<sln::PixelRGB_8u> img_border_access({img.width(), img.height()});

  for (auto y = 0_idx; y < img.height(); ++y)
  {
    for (auto x = 0_idx; x < img.width(); ++x)
    {
      img_border_access(x, y) = sln::get<sln::BorderAccessMode::Replicated>(img, sln::PixelIndex{x + 100}, sln::PixelIndex{y - 200});
    }
  }

  std::cout << "Writing the result to disk: '" << output_filename_interpolated << "'...\n";
  write_image(sln::to_dyn_image_view(img_interpolation), sln::ImageFormat::PNG,
              sln::FileWriter(output_filename_interpolated));

  std::cout << "Writing the result to disk: '" << output_filename_border_access << "'...\n";
  write_image(sln::to_dyn_image_view(img_border_access), sln::ImageFormat::PNG,
              sln::FileWriter(output_filename_border_access));

  return 0;
}
