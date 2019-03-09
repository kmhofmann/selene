// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Clone.hpp>
#include <selene/img_ops/ImageConversions.hpp>
#include <selene/img_ops/View.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_clone_crop = "bike_duck_clone_crop.png";
constexpr auto output_filename_view_crop = "bike_duck_clone_view.png";

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

  std::cout << "Cloning and creating views...\n";

  // We can clone (copy) the complete image...

  auto img_clone = sln::clone(img);
  assert(sln::equal(img_clone, img));

  // ...or we can create a view onto the data of `img`.

  auto img_view = sln::view(img);
  assert(sln::equal(img_view, img));

  // Cloning a sub-region (from an image or image view) is as easy as this:

  auto img_clone_crop = sln::clone(img_view, {130_idx, 100_idx, 250_px, 220_px});
  assert(img_clone_crop.width() == 250_px);
  assert(img_clone_crop.height() == 220_px);

  // Similarly for a view onto a sub-region:

  const auto img_view_crop = sln::view(img, {130_idx, 100_idx, 250_px, 220_px});
  assert(img_view_crop.width() == 250_px);
  assert(img_view_crop.height() == 220_px);

  assert(sln::equal(img_clone_crop, img_view_crop));

  // Let's still write both crops to disk.

  std::cout << "Writing the result to disk: '" << output_filename_clone_crop << "'...\n";
  sln::write_image(sln::to_dyn_image_view(img_clone_crop), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename_clone_crop));

  std::cout << "Writing the result to disk: '" << output_filename_view_crop << "'...\n";
  sln::write_image(sln::to_dyn_image_view(img_view_crop), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename_view_crop));

  return 0;
}
