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

constexpr auto output_filename_y = "bike_duck_converted_to_grayscale.png";
constexpr auto output_filename_rgba = "bike_duck_converted_to_rgba.png";
constexpr auto output_filename_bgr = "bike_duck_converted_to_bgr.png";

int main(int argc, char** argv)
{
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  const auto img_rgb = sln_examples::read_example_image<sln::Pixel_8u3>("bike_duck.png", data_path);

  std::cout << "Converting the image from RGB to Y (grayscale)...\n";
  const auto img_y = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::Y>(img_rgb);

  std::cout << "Writing the result to disk: '" << output_filename_y << "'...\n";
  sln::write_image(sln::to_image_data_view(img_y, sln::PixelFormat::Y), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename_y));

  std::cout << "Converting the image from RGB to RGBA (slightly transparent)...\n";
  const auto img_rgba = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::RGBA>(img_rgb, std::uint8_t{192});

  std::cout << "Writing the result to disk: '" << output_filename_rgba << "'...\n";
  sln::write_image(sln::to_image_data_view(img_rgba, sln::PixelFormat::RGBA), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename_rgba));

  std::cout << "Converting the image from RGB to BGR...\n";
  const auto img_bgr = sln::convert_image<sln::PixelFormat::RGB, sln::PixelFormat::BGR>(img_rgb);

  std::cout << "Writing the result to disk: '" << output_filename_bgr << "'...\n";
  sln::write_image(sln::to_image_data_view(img_bgr, sln::PixelFormat::BGR), sln::ImageFormat::PNG,
                   sln::FileWriter(output_filename_bgr));

  return 0;
}