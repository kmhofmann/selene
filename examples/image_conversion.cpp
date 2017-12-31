// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/img/ImageConversions.hpp>

#include <selene/img/ImageToImageData.hpp>
#include <selene/img/IO.hpp>

#include <selene/io/FileWriter.hpp>

#include <iostream>
#include <string>

#include "Utils.hpp"

int main(int argc, char** argv)
{
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  const auto img_rgb = sln_examples::read_example_image<sln::Pixel_8u3>("bike_duck.png", data_path);

  std::cout << "We are converting the image from RGB to Y (grayscale)...\n";
  // TODO: can't be const because it's passed on to to_image_data_view below which expects a non-const reference;
  // TODO: can we do better?
  auto img_y = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_rgb);

  const std::string output_filename = "bike_duck_converted_to_grayscale.png";
  std::cout << "...and are writing the result to disk: '" << output_filename << "'\n";
  sln::write_image(sln::to_image_data_view(img_y, sln::PixelFormat::Y), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename));

  return 0;
}