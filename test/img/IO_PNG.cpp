// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <catch.hpp>

#include <cstdlib>

#include <boost/filesystem.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/FileWriter.hpp>
#include <selene/io/FileUtils.hpp>
#include <selene/io/MemoryReader.hpp>
#include <selene/io/VectorWriter.hpp>

#include <selene/img/Image.hpp>
#include <selene/img/ImageData.hpp>
#include <selene/img/ImageDataToImage.hpp>
#include <selene/img/ImageToImageData.hpp>
#include <selene/img/PNGRead.hpp>
#include <selene/img/PNGWrite.hpp>

#include <Utils.hpp>

using namespace selene;
using namespace selene::img;
using namespace selene::io;

namespace fs = boost::filesystem;

constexpr auto ref_width = 1024;
constexpr auto ref_height = 684;
constexpr std::array<std::array<int, 6>, 3> px = {
    {std::array<int, 6>{{226, 180, 244, 198, 0, 203}},
     std::array<int, 6>{{582, 415, 228, 227, 233, 227}},
     std::array<int, 6>{{878, 597, 57, 60, 69, 61}}}}; // {x, y}, {r, g, b}, {y}

namespace {

fs::path in_filename()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "bike_duck.png" : fs::path("../data/bike_duck.png");
}

} // namespace _

TEST_CASE("PNG image reading and writing, no conversion", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading without conversion
  MessageLog messages_read;
  auto img_data = read_png(source, PNGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }

  FileWriter sink((tmp_path / "test_duck.png").c_str());
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  MessageLog messages_write;
  bool status_write = write_png(sink, to_image_data_view(img, PixelFormat::RGB), PNGCompressionOptions(),
                                &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG image reading and writing, conversion to grayscale", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading with conversion to grayscale
  PNGDecompressionOptions decompression_options;
  decompression_options.convert_rgb_to_gray = true;
  MessageLog messages_read;
  auto img_data = read_png(source, decompression_options, &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 1);
  REQUIRE(img_data.nr_channels() == 1);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u1>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 1);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(static_cast<int>(img(px[i][0], px[i][1])) == static_cast<int>(Pixel_8u1(px[i][5])));
  }

  FileWriter sink((tmp_path / "test_duck_gray.png").c_str());
  REQUIRE(sink.is_open());

  // Test writing of grayscale image
  MessageLog messages_write;
  bool status_write = write_png(sink, to_image_data_view(img, PixelFormat::Y), PNGCompressionOptions(), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG image reading and writing, reusing decompression object", "[img]")
{
  const auto tmp_path = get_tmp_path();
  FileReader source(in_filename().c_str());
  REQUIRE(source.is_open());

  // Test reading of header...
  PNGDecompressionObject decompression_object;
  const auto header = read_png_header(decompression_object, source);
  REQUIRE(header.width == ref_width);
  REQUIRE(header.height == ref_height);
  REQUIRE(header.nr_channels == 3);
  REQUIRE(header.bit_depth == 8);

  // ...and then reusing decompression object/header info for reading the image
  MessageLog messages_read;
  auto img_data = read_png(decompression_object, source, PNGDecompressionOptions(), &messages_read, &header);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }
}

TEST_CASE("PNG image reading and writing, reading/writing from/to memory", "[img]")
{
  const auto tmp_path = get_tmp_path();
  const auto file_contents = read_file_contents(in_filename().c_str());
  REQUIRE(!file_contents.empty());

  // Test reading from memory
  MemoryReader source(file_contents.data(), file_contents.size());
  REQUIRE(source.is_open());

  // Test reading without conversion
  MessageLog messages_read;
  auto img_data = read_png(source, PNGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = to_image<Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    REQUIRE(img(px[i][0], px[i][1]) == Pixel_8u3(px[i][2], px[i][3], px[i][4]));
  }

  // Test writing to memory
  std::vector<std::uint8_t> compressed_data;
  VectorWriter sink(compressed_data);
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  MessageLog messages_write;
  bool status_write = write_png(sink, to_image_data_view(img, PixelFormat::RGB), PNGCompressionOptions(),
                                &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

#endif // defined(SELENE_WITH_LIBPNG)
