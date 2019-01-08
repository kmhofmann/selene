// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/Kernel.hpp>
#include <selene/base/io/FileWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Convolution.hpp>

#include <cassert>
#include <iostream>
#include <string>

#include "Utils.hpp"

constexpr auto output_filename_convolved_x = "stickers_convolved_x.jpg";
constexpr auto output_filename_convolved_y = "stickers_convolved_y.jpg";
constexpr auto output_filename_convolved_yx = "stickers_convolved_yx.jpg";

using namespace sln::literals;

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  // Read in the example image (check the implementation in Utils.hpp);
  // `PixelRGB_8u` designates 3 channels (R, G, B) of unsigned 8-bit data for each pixel.

  auto img = sln_examples::read_example_image<sln::PixelRGB_8u>("stickers.png", data_path);
  assert(img.width() == 400_px);
  assert(img.height() == 320_px);

  // Determine access mode around image borders: replicate
  const auto access_mode = sln::BorderAccessMode::Replicated;

  // Create a floating point kernel (sigma=1.0; size covering 3.0 standard deviations).
  sln::Kernel<double> kernel_fp = sln::gaussian_kernel(1.0, 3.0);
  // (This function creates a dynamically sized kernel; there is also a similar function that creates a Gaussian
  // kernel of a specific size: e.g. sln::gaussian_kernel<7>(1.0) creates a kernel with 7 elements.)

  // Perform a convolution on the image using this kernel, in x-direction
  const auto img_convolved_x = sln::convolution_x<access_mode>(img, kernel_fp);
  // Perform a convolution on the image using this kernel, in y-direction
  const auto img_convolved_y = sln::convolution_y<access_mode>(img, kernel_fp);

  // Create an integer based kernel based on the above kernel
  constexpr auto shift = 8u;  // We are going to shift this many bits left for an integral representation.
  sln::Kernel<std::int32_t> kernel_int = sln::integer_kernel<std::int32_t, sln::power(2, shift)>(kernel_fp);

  // Perform a convolution on the image using the integer kernel, in both directions.
  // With the additional 'shift' parameter, the result is shifted this many bits to the right before it is written
  // to the output image.
  const auto img_tmp = sln::convolution_y<access_mode, shift>(img, kernel_int);
  const auto img_convolved_yx = sln::convolution_x<access_mode, shift>(img_tmp, kernel_int);

  std::cout << "Writing the result to disk: '" << output_filename_convolved_x << "'...\n";
  sln::write_image(sln::to_dyn_image_view(img_convolved_x), sln::ImageFormat::JPEG,
                   sln::FileWriter(output_filename_convolved_x));

  std::cout << "Writing the result to disk: '" << output_filename_convolved_y << "'...\n";
  sln::write_image(sln::to_dyn_image_view(img_convolved_y), sln::ImageFormat::JPEG,
                   sln::FileWriter(output_filename_convolved_y));

  std::cout << "Writing the result to disk: '" << output_filename_convolved_yx << "'...\n";
  sln::write_image(sln::to_dyn_image_view(img_convolved_yx), sln::ImageFormat::JPEG,
                   sln::FileWriter(output_filename_convolved_yx));

  return 0;
}
