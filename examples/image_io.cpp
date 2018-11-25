// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/MemoryReader.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/old_img/ImageTypeAliases.hpp>
#include <selene/old_img/ImageToImageData.hpp>
#include <selene/old_img_io/IO.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_png = "bike_duck.png";

using namespace sln;  // Never outside of example code

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  const auto example_img_path = sln_examples::full_data_path("bike_duck.jpg", data_path);

  // First, we will read an image from a file on disk.

  std::cout << "Reading the example image data from file '" << example_img_path.string() << "'...\n";
  ImageData<> img_data_0 = read_image(FileReader(example_img_path.string()));

  if (!img_data_0.is_valid())
  {
    sln_examples::print_help_and_exit("Image data could not be decoded.");
  }

  // Ensure the decoded image data has 3 channels
  assert(img_data_0.nr_channels() == 3);
  // Ensure the decoded image data is 8-bit per channel
  assert(img_data_0.nr_bytes_per_channel() == 1);

#ifndef NDEBUG
  const auto img_data_0_width = img_data_0.width();
  const auto img_data_0_height = img_data_0.height();
#endif  // NDEBUG

  // If the read image data has the correct properties, we can convert it to a strongly typed instance of
  // `Image<PixelRGB_8u>` to potentially perform further data manipulations.
  // In this case, `PixelRGB_8u` designates 3 channels of unsigned 8-bit data for each pixel.

  std::cout << "Converting the ImageData<> instance to a (strongly typed) Image<PixelRGB_8u>...\n";
  const Image<PixelRGB_8u> img = to_image<PixelRGB_8u>(std::move(img_data_0));

  // Since the data was moved into the `Image<>` instance, the contents of `img_data_0` are now invalid!
  // But the new, strongly typed image is.

  assert(img.is_valid());
  assert(img.width() == img_data_0_width);
  assert(img.height() == img_data_0_height);

  // Now we will simply write the image to disk again. To do this, we convert back to an `ImageData` instance;
  // in this case a view onto constant image data (also since we declared `old_img` const above).

  const ImageData<ImageDataStorage::Constant> img_data_1 = to_image_data_view(img);

  std::cout << "Writing the image to disk again...\n";
  write_image(img_data_1, ImageFormat::PNG, FileWriter(output_filename_png));

  // But we can also write to memory; represented by a std::vector<std::uint8_t>.

  std::cout << "Writing the image to memory...\n";
  std::vector<std::uint8_t> encoded_png_data;
  write_image(img_data_1, ImageFormat::PNG, VectorWriter(encoded_png_data));

  assert(!encoded_png_data.empty());

  // And we can decode the image from memory again. The encoded image data does not need to come from disk.

  std::cout << "Reading the image from memory...\n";
  ImageData<> img_data_2 = read_image(MemoryReader(encoded_png_data.data(), encoded_png_data.size()));
  const Image<PixelRGB_8u> img_2 = to_image<PixelRGB_8u>(std::move(img_data_2));

  // And of course the resulting image is identical to the one previously read from disk

  assert(img == img_2);

  return 0;
}
