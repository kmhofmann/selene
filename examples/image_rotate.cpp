// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Clone.hpp>
#include <selene/img_ops/Transformations.hpp>

#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_transposed = "bike_duck_transp.jpg";

constexpr auto output_filename_flip_h = "bike_duck_flip_h.jpg";
constexpr auto output_filename_flip_v = "bike_duck_flip_v.jpg";
constexpr auto output_filename_flip_b = "bike_duck_flip_b.jpg";
constexpr auto output_filename_flip_h_in_place = "bike_duck_flip_h_in_place.jpg";
constexpr auto output_filename_flip_v_in_place = "bike_duck_flip_v_in_place.jpg";

constexpr auto output_filename_rot_cw_090 = "bike_duck_rot_cw_090.jpg";
constexpr auto output_filename_rot_cw_180 = "bike_duck_rot_cw_180.jpg";
constexpr auto output_filename_rot_cw_270 = "bike_duck_rot_cw_270.jpg";

constexpr auto output_filename_rot_ccw_090 = "bike_duck_rot_ccw_090.jpg";
constexpr auto output_filename_rot_ccw_180 = "bike_duck_rot_ccw_180.jpg";
constexpr auto output_filename_rot_ccw_270 = "bike_duck_rot_ccw_270.jpg";

using namespace sln::literals;

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `Pixel_8u3` designates 3 channels of unsigned 8-bit data for each pixel.

  const auto img = sln_examples::read_example_image<sln::Pixel_8u3>("bike_duck.png", data_path);

  std::cout << "Transposing image...\n";
  const auto img_transp = sln::transpose(img);

  std::cout << "Flipping image...\n";
  const auto img_flip_h = sln::flip<sln::FlipDirection::Horizontal>(img);
  const auto img_flip_v = sln::flip<sln::FlipDirection::Vertical>(img);
  const auto img_flip_b = sln::flip<sln::FlipDirection::Both>(img);
  auto img_flip_h_in_place = sln::clone(img);
  sln::flip_horizontally_in_place(img_flip_h_in_place);
  auto img_flip_v_in_place = sln::clone(img);
  sln::flip_vertically_in_place(img_flip_v_in_place);

  std::cout << "Rotating image clockwise...\n";
  const auto img_rot_cw_090 = sln::rotate<sln::RotationDirection::Clockwise90>(img);
  const auto img_rot_cw_180 = sln::rotate<sln::RotationDirection::Clockwise180>(img);
  const auto img_rot_cw_270 = sln::rotate<sln::RotationDirection::Clockwise270>(img);

  std::cout << "Rotating image counterclockwise...\n";
  const auto img_rot_ccw_090 = sln::rotate<sln::RotationDirection::Counterclockwise90>(img);
  const auto img_rot_ccw_180 = sln::rotate<sln::RotationDirection::Counterclockwise180>(img);
  const auto img_rot_ccw_270 = sln::rotate<sln::RotationDirection::Counterclockwise270>(img);

  auto write = [](const auto& img, auto filename){
    std::cout << "Writing the result to disk: '" << filename << "'...\n";
    write_image(sln::to_dyn_image_view(img, sln::PixelFormat::RGB), sln::ImageFormat::JPEG,
                sln::FileWriter(filename), nullptr, 80);
  };

  write(img_transp, output_filename_transposed);

  write(img_flip_h, output_filename_flip_h);
  write(img_flip_v, output_filename_flip_v);
  write(img_flip_b, output_filename_flip_b);
  write(img_flip_h_in_place, output_filename_flip_h_in_place);
  write(img_flip_v_in_place, output_filename_flip_v_in_place);

  write(img_rot_cw_090, output_filename_rot_cw_090);
  write(img_rot_cw_180, output_filename_rot_cw_180);
  write(img_rot_cw_270, output_filename_rot_cw_270);

  write(img_rot_ccw_090, output_filename_rot_ccw_090);
  write(img_rot_ccw_180, output_filename_rot_ccw_180);
  write(img_rot_ccw_270, output_filename_rot_ccw_270);

  return 0;
}
