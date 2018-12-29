#include <selene/base/io/FileUtils.hpp>

#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/ImageTypeAliases.hpp>

#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/IO.hpp>

#include <selene/img_ops/Algorithms.hpp>
#include <selene/img_ops/Clone.hpp>
#include <selene/img_ops/Convolution.hpp>
#include <selene/img_ops/ImageConversions.hpp>
#include <selene/img_ops/Transformations.hpp>
#include <selene/img_ops/View.hpp>

#include <cassert>

#include "Utils.hpp"

using namespace sln;  // Only for the README file. Don't do this otherwise.

int main(int argc, char** argv)
{
  // Read data path as optional command line argument
  const char* data_path = (argc >= 2) ? argv[1] : nullptr;

  const auto example_img_path = sln_examples::full_data_path("bike_duck.jpg", data_path);

  // Decode JPEG image data from disk
  DynImage img_data = read_image(FileReader(example_img_path.string()));
  assert(img_data.nr_channels() == 3 && img_data.nr_bytes_per_channel() == 1);

  // Convert to strongly typed RGB image
  Image<PixelRGB_8u> img_rgb = to_image<PixelRGB_8u>(std::move(img_data));
  assert(img_rgb.width() > 400_px && img_rgb.height() > 350_px);

  // Create non-owning view on part of the image
  MutableImageView<PixelRGB_8u> img_part = view(img_rgb, {100_idx, 100_idx, 300_px, 250_px});

  // Darken this part
  for_each_pixel(img_part, [](auto& px){ px /= 4; });

  // Flip this part horizontally
  flip_horizontally_in_place(img_part);

  // Apply a convolution to this part (1-D Gaussian kernel in x-direction, sigma=5.0, range: 3 standard deviations)
  Kernel<double> kernel = gaussian_kernel(5.0, 3.0);
  const auto img_part_copy = convolution_x<BorderAccessMode::Unchecked>(img_part, kernel);

  // And copy the result back to the original image (i.e. to the view).
  clone(img_part_copy, img_part);

  // Convert whole image to RGBA, adding semi-transparent alpha channel
  const Image<PixelRGBA_8u> img_rgba =
      convert_image<PixelFormat::RGBA>(img_rgb, std::uint8_t{192});

  // Encode in-memory to PNG
  std::vector<std::uint8_t> encoded_png_data;
  write_image(to_dyn_image_view(img_rgba), ImageFormat::PNG,
              VectorWriter(encoded_png_data));

  // Write encoded binary data to disk (or do something else with it...)
  write_data_contents("example_out.png", encoded_png_data);
}
