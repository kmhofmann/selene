// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/ImageTypeAliases.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img_io/IO.hpp>
#include <selene/img_ops/Resample.hpp>

#include <selene/io/FileWriter.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_resampled_0 = "bike_duck_bilinear_resampled_0.jpg";
constexpr auto output_filename_resampled_1 = "bike_duck_bilinear_resampled_1.jpg";

using namespace sln;  // Never outside of example code

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `PixelRGB_8u` designates 3 channels (R, G, B) of unsigned 8-bit data for each pixel.

  auto img = sln_examples::read_example_image<PixelRGB_8u>("bike_duck.png", data_path);
  assert(img.width() == 1024_px);
  assert(img.height() == 684_px);

  // Resample the image to a much smaller size. This results in plenty of aliasing.
  const auto img_resampled_0 = resample(img, 256_px, 171_px);

  // Afterwards, resample to its original size.
  const auto img_resampled_1 = resample(img_resampled_0, img.width(), img.height());

  std::cout << "Writing the result to disk: '" << output_filename_resampled_0 << "'...\n";
  write_image(to_image_data_view(img_resampled_0), ImageFormat::JPEG,
              FileWriter(output_filename_resampled_0));

  std::cout << "Writing the result to disk: '" << output_filename_resampled_1 << "'...\n";
  write_image(to_image_data_view(img_resampled_1), ImageFormat::JPEG,
              FileWriter(output_filename_resampled_1));

  return 0;
}
