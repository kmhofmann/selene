// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/ImageConversions.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img/IO.hpp>

#include <selene/io/FileWriter.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_clone_crop = "bike_duck_clone_crop.png";
constexpr auto output_filename_view_crop = "bike_duck_clone_view.png";

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

  std::cout << "Cloning and creating views...\n";

  // We can clone (copy) the complete image...

  auto img_clone = clone(img);
  assert(img_clone == img);

  // ...or we can create a view onto the data of `img`.

  auto img_view = view(img);
  assert(img_view == img);

  // Cloning a sub-region is as easy as this:

  auto img_clone_crop = clone(img, 130_px, 100_px, 250_px, 220_px);
  assert(!img_clone_crop.is_view());
  assert(img_clone_crop.width() == 250_px);
  assert(img_clone_crop.height() == 220_px);

  // Similarly for a view onto a sub-region:

  const auto img_view_crop = view(img, 130_px, 100_px, 250_px, 220_px);
  assert(img_view_crop.is_view());
  assert(img_view_crop.width() == 250_px);
  assert(img_view_crop.height() == 220_px);

  assert(img_clone_crop == img_view_crop);

  // Let's still write both crops to disk.

  std::cout << "Writing the result to disk: '" << output_filename_clone_crop << "'...\n";
  write_image(to_image_data_view(img_clone_crop, PixelFormat::RGB), ImageFormat::PNG,
              FileWriter(output_filename_clone_crop));

  std::cout << "Writing the result to disk: '" << output_filename_view_crop << "'...\n";
  write_image(to_image_data_view(img_view_crop, PixelFormat::RGB), ImageFormat::PNG,
              FileWriter(output_filename_view_crop));

  return 0;
}
