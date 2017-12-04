// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <catch.hpp>

#include <cstdlib>
#include <cstring>

#include <boost/filesystem.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/FileUtils.hpp>
#include <selene/io/FileWriter.hpp>
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
    {std::array<int, 6>{{226, 180, 244, 198, 0, 203}}, std::array<int, 6>{{582, 415, 228, 227, 233, 227}},
     std::array<int, 6>{{878, 597, 57, 60, 69, 61}}}};  // {x, y}, {r, g, b}, {y}

namespace {

fs::path in_filename()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "bike_duck.png" : fs::path("../data/bike_duck.png");
}

fs::path test_suite_dir()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "png_suite" : fs::path("../data/png_suite");
}

void check_write_read(ImageData& img_data, const boost::filesystem::path& tmp_path)
{
  // Write as PNG file...
  FileWriter sink((tmp_path / "test_img.png").c_str());
  REQUIRE(sink.is_open());
  MessageLog messages_write;
  bool status_write = write_png(img_data, sink, PNGCompressionOptions(), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
  sink.close();
  REQUIRE(!sink.is_open());

  // ...and read again
  FileReader source((tmp_path / "test_img.png").c_str());
  MessageLog messages_read;
  auto img_data_2 = read_png(source, PNGDecompressionOptions(), &messages_read);
  REQUIRE(messages_read.messages().empty());
  source.close();
  REQUIRE(!source.is_open());

  // Make sure that content and state is the same
  REQUIRE(img_data_2.is_valid());
  REQUIRE(img_data_2.width() == img_data.width());
  REQUIRE(img_data_2.height() == img_data.height());
  REQUIRE(img_data_2.stride_bytes() == img_data.stride_bytes());
  REQUIRE(img_data_2.nr_channels() == img_data.nr_channels());
  REQUIRE(img_data_2.nr_bytes_per_channel() == img_data.nr_bytes_per_channel());
  REQUIRE(img_data_2.pixel_format() == img_data.pixel_format());
  REQUIRE(img_data_2.sample_format() == img_data.sample_format());
  REQUIRE(img_data_2.total_bytes() == img_data.total_bytes());

  const auto nr_bytes_per_row = img_data_2.width() * img_data_2.nr_channels() * img_data_2.nr_bytes_per_channel();
  for (Index y = 0; y < img_data_2.height(); ++y)
  {
    REQUIRE(std::memcmp(img_data_2.byte_ptr(y), img_data.byte_ptr(y), nr_bytes_per_row) == 0);
  }
}

}  // namespace

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
  bool status_write = write_png(to_image_data_view(img, PixelFormat::RGB), sink, PNGCompressionOptions(),
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
  bool status_write = write_png(to_image_data_view(img, PixelFormat::Y), sink, PNGCompressionOptions(),
                                &messages_write);
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
  bool status_write = write_png(to_image_data_view(img, PixelFormat::RGB), sink, PNGCompressionOptions(),
                                &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG reading of the official test suite", "[img]")
{
  const auto tmp_path = get_tmp_path();
  const auto test_suite_path = test_suite_dir();
  for (boost::filesystem::directory_entry& e : boost::filesystem::directory_iterator(test_suite_path))
  {
    if (e.path().extension() == ".png")
    {
      FileReader source(e.path().c_str());
      REQUIRE(source.is_open());

      MessageLog messages_read;
      auto img_data = read_png(source, PNGDecompressionOptions(), &messages_read);

      const auto filename_first_letter = e.path().stem().c_str()[0];
      const auto is_broken = (filename_first_letter == 'x');  // Broken image files begin with 'x'

      if (!is_broken)
      {
        // REQUIRE(messages_read.messages().empty());  // Might not be guaranteed, so this is disabled
        REQUIRE(img_data.width() > 0);
        REQUIRE(img_data.height() > 0);
        REQUIRE(img_data.stride_bytes() > 0);
        REQUIRE(img_data.nr_channels() > 0);
        REQUIRE(img_data.nr_bytes_per_channel() > 0);
        REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
        REQUIRE(img_data.is_packed());
        REQUIRE(!img_data.is_view());
        REQUIRE(!img_data.is_empty());
        REQUIRE(img_data.is_valid());

        check_write_read(img_data, tmp_path);
      }
      else
      {
        REQUIRE(!messages_read.messages().empty());
        REQUIRE(!img_data.is_valid());
      }
    }
  }
}

#endif  // defined(SELENE_WITH_LIBPNG)
