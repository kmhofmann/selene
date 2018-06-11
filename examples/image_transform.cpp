// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/ImageTypeAliases.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img_io/IO.hpp>
#include <selene/img_ops/Algorithms.hpp>

#include <selene/io/FileWriter.hpp>

#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_transformed = "bike_duck_transformed.png";

using namespace sln;  // Never outside of example code

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `Pixel_8u3` designates 3 channels of unsigned 8-bit data for each pixel.

  const auto img_rgb = sln_examples::read_example_image<PixelRGB_8u>("bike_duck.png", data_path);

  // Transform the image from 8-bit integral to 32-bit floating point type, and normalize values to be within (0...1)
  std::cout << "Transforming the image from 8-bit integral to 32-bit floating point type (0...1)...\n";
  auto img_f = transform_pixels<PixelRGB_32f>(img_rgb, [](const auto& px) { return PixelRGB_32f(px) / 255.0; });

  // Play around with the color channels
  sln::for_each_pixel(img_f, [](auto& px) {
    px[0] = std::pow(px[0], 0.1);
    px[1] = std::pow(px[1], 0.8);
    px[2] = std::pow(px[2], 0.3);
  });

  // Transform the image back to 8-bit integral representation (0...255)
  auto img_transf = transform_pixels<PixelRGB_8u>(img_f, [](const auto& px) { return PixelRGB_8u(px * 255.0); });

  // Write out the transformed image to disk
  std::cout << "Writing the result to disk: '" << output_filename_transformed << "'...\n";
  write_image(to_image_data_view(img_transf), ImageFormat::PNG,
              FileWriter(output_filename_transformed));

  return 0;
}
