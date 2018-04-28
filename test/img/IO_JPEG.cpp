// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBJPEG)

#include <catch.hpp>

#include <cstdlib>

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
#include <selene/img/JPEGRead.hpp>
#include <selene/img/JPEGWrite.hpp>

#include <Utils.hpp>

namespace fs = boost::filesystem;
using namespace sln::literals;

// clang-format off
constexpr auto ref_width = 1024u;
constexpr auto ref_height = 684u;
constexpr std::array<std::array<unsigned int, 6>, 3> pix = {
    {std::array<unsigned int, 6>{{226, 180, 244, 198, 0, 189}},
     std::array<unsigned int, 6>{{582, 415, 228, 227, 232, 228}},
     std::array<unsigned int, 6>{{878, 597, 57, 60, 69, 60}}}};  // {x, y}, {r, g, b}, {y}
constexpr auto compression_factor = 70;
// clang-format on

namespace {

fs::path in_filename()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "bike_duck.jpg" : fs::path("../data/bike_duck.jpg");
}

}  // namespace

TEST_CASE("JPEG image reading and writing, no conversion", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  // Test reading without conversion
  sln::FileReader source(in_filename().string());
  REQUIRE(source.is_open());
  sln::MessageLog messages_read;
  auto img_data = sln::read_jpeg(source, sln::JPEGDecompressionOptions(), &messages_read);
  source.close();
  REQUIRE(!source.is_open());

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

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    const auto x = sln::PixelIndex(pix[i][0]);
    const auto y = sln::PixelIndex(pix[i][1]);
    REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
  }

  // Test writing of RGB image
  sln::FileWriter sink((tmp_path / "test_duck.jpg").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_jpeg(sln::to_image_data_view(img, sln::PixelFormat::RGB), sink,
                                      sln::JPEGCompressionOptions(compression_factor), &messages_write);
  sink.close();
  REQUIRE(!sink.is_open());

  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("JPEG image reading and writing, conversion to grayscale", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  // Test reading with conversion to grayscale
  sln::FileReader source(in_filename().string());
  REQUIRE(source.is_open());
  sln::MessageLog messages_read;
  auto img_data = sln::read_jpeg(source, sln::JPEGDecompressionOptions(sln::JPEGColorSpace::Grayscale), &messages_read);
  source.close();
  REQUIRE(!source.is_open());

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

  auto img = sln::to_image<sln::Pixel_8u1>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 1);
  for (int i = 0; i < 3; ++i)
  {
    const auto x = sln::PixelIndex(pix[i][0]);
    const auto y = sln::PixelIndex(pix[i][1]);
    REQUIRE(static_cast<int>(img(x, y)) == static_cast<int>(sln::Pixel_8u1(pix[i][5])));
  }

  // Test writing of grayscale image
  sln::FileWriter sink((tmp_path / "test_duck_gray.jpg").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_jpeg(sln::to_image_data_view(img, sln::PixelFormat::Y), sink,
                                      sln::JPEGCompressionOptions(compression_factor), &messages_write);
  sink.close();
  REQUIRE(!sink.is_open());

  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());

  // Test reading of grayscale JPEG again
  sln::FileReader source_2((tmp_path / "test_duck_gray.jpg").string());
  REQUIRE(source_2.is_open());
  sln::MessageLog messages_read_2;
  auto img_data_2 = sln::read_jpeg(source_2, sln::JPEGDecompressionOptions(), &messages_read_2);
  source_2.close();
  REQUIRE(!source_2.is_open());

  REQUIRE(messages_read_2.messages().empty());
  REQUIRE(img_data_2.width() == ref_width);
  REQUIRE(img_data_2.height() == ref_height);
  REQUIRE(img_data_2.stride_bytes() == ref_width * 1);
  REQUIRE(img_data_2.nr_channels() == 1);
  REQUIRE(img_data_2.nr_bytes_per_channel() == 1);
  REQUIRE(img_data_2.total_bytes() == img_data_2.stride_bytes() * img_data_2.height());
  REQUIRE(img_data_2.is_packed());
  REQUIRE(!img_data_2.is_view());
  REQUIRE(!img_data_2.is_empty());
  REQUIRE(img_data_2.is_valid());
}

TEST_CASE("JPEG image reading and writing, reusing decompression object", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  // Test reading of header...
  sln::FileReader source(in_filename().string());
  REQUIRE(source.is_open());
  sln::JPEGDecompressionObject decompression_object;
  const auto header = sln::read_jpeg_header(decompression_object, source);

  REQUIRE(header.width == ref_width);
  REQUIRE(header.height == ref_height);
  REQUIRE(header.nr_channels == 3);
  REQUIRE(header.color_space == sln::JPEGColorSpace::YCbCr);

  // ...and then reusing decompression object/header info for reading the image
  sln::MessageLog messages_read;
  auto img_data = sln::read_jpeg(decompression_object, source, sln::JPEGDecompressionOptions(), &messages_read,
                                 &header);
  source.close();
  REQUIRE(!source.is_open());

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

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    const auto x = sln::PixelIndex(pix[i][0]);
    const auto y = sln::PixelIndex(pix[i][1]);
    REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
  }
}

#if defined(SELENE_LIBJPEG_PARTIAL_DECODING)
TEST_CASE("JPEG image reading and writing, partial image reading", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  // Test reading of partial image
  const auto expected_width = 404_px;
  const auto targeted_height = 350_px;
  sln::BoundingBox region(100_px, 100_px, 400_px, targeted_height);

  sln::FileReader source(in_filename().string());
  REQUIRE(source.is_open());
  sln::MessageLog messages_read;
  auto img_data = sln::read_jpeg(source, sln::JPEGDecompressionOptions(sln::JPEGColorSpace::Auto, region),
                                 &messages_read);
  source.close();
  REQUIRE(!source.is_open());

  REQUIRE(messages_read.messages().empty());
  REQUIRE(img_data.width() == expected_width);
  REQUIRE(img_data.height() == targeted_height);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.stride_bytes() == expected_width * 3);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(!img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == expected_width);
  REQUIRE(img.height() == targeted_height);
  REQUIRE(img.stride_bytes() == expected_width * 3);

  // Test writing of RGB image
  sln::FileWriter sink((tmp_path / "test_duck_crop.jpg").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_jpeg(sln::to_image_data_view(img, sln::PixelFormat::RGB), sink,
                                      sln::JPEGCompressionOptions(compression_factor), &messages_write);
  sink.close();
  REQUIRE(!sink.is_open());

  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());

  // Test reading of JPEG again
  sln::FileReader source_2((tmp_path / "test_duck_crop.jpg").string());
  REQUIRE(source_2.is_open());
  sln::MessageLog messages_read_2;
  auto img_data_2 = sln::read_jpeg(source_2, sln::JPEGDecompressionOptions(), &messages_read_2);
  source_2.close();
  REQUIRE(!source_2.is_open());

  REQUIRE(messages_read_2.messages().empty());
  REQUIRE(img_data_2.width() == expected_width);
  REQUIRE(img_data_2.height() == targeted_height);
  REQUIRE(img_data_2.stride_bytes() == expected_width * 3);
  REQUIRE(img_data_2.nr_channels() == 3);
  REQUIRE(img_data_2.nr_bytes_per_channel() == 1);
  REQUIRE(img_data_2.total_bytes() == img_data_2.stride_bytes() * img_data_2.height());
  REQUIRE(img_data_2.is_packed());
  REQUIRE(!img_data_2.is_view());
  REQUIRE(!img_data_2.is_empty());
  REQUIRE(img_data_2.is_valid());
}
#endif

TEST_CASE("JPEG image reading and writing, reading/writing from/to memory", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto file_contents = sln::read_file_contents(in_filename().string());
  REQUIRE(!file_contents.empty());

  // Test reading from memory
  sln::MemoryReader source(file_contents.data(), file_contents.size());
  REQUIRE(source.is_open());
  sln::MessageLog messages_read;
  auto img_data = sln::read_jpeg(source, sln::JPEGDecompressionOptions(), &messages_read);
  source.close();
  REQUIRE(!source.is_open());

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

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(img_data));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (int i = 0; i < 3; ++i)
  {
    const auto x = sln::PixelIndex(pix[i][0]);
    const auto y = sln::PixelIndex(pix[i][1]);
    REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
  }

  // Test writing to memory
  std::vector<std::uint8_t> compressed_data;
  sln::VectorWriter sink(compressed_data);
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  sln::MessageLog messages_write;
  bool status_write = sln::write_jpeg(sln::to_image_data_view(img, sln::PixelFormat::RGB), sink,
                                      sln::JPEGCompressionOptions(95), &messages_write);
  sink.close();
  REQUIRE(!sink.is_open());

  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
  REQUIRE(compressed_data.size() > 80000);  // conservative lower bound estimate; should be around 118000
}

TEST_CASE("JPEG image reading, through JPEGReader interface", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  sln::FileReader source(in_filename().string());
  REQUIRE(source.is_open());
  sln::JPEGReader<sln::FileReader> jpeg_reader(source, sln::JPEGDecompressionOptions());

  const auto header = jpeg_reader.read_header();
  REQUIRE(header.is_valid());
  REQUIRE(header.width == ref_width);
  REQUIRE(header.height == ref_height);
  REQUIRE(header.nr_channels == 3);
  REQUIRE(header.color_space == sln::JPEGColorSpace::YCbCr);

  jpeg_reader.set_decompression_options(sln::JPEGDecompressionOptions());
  const auto info = jpeg_reader.get_output_image_info();
  REQUIRE(info.is_valid());
  REQUIRE(info.width == ref_width);
  REQUIRE(info.height == ref_height);
  REQUIRE(info.nr_channels == 3);
  REQUIRE(info.color_space == sln::JPEGColorSpace::RGB);

  auto memory_block = sln::AlignedNewAllocator::allocate(info.required_bytes(), 16);
  sln::ImageData<> img_data(memory_block.data(), info.width, info.height, info.nr_channels, info.nr_bytes_per_channel());
  auto res = jpeg_reader.read_image_data(img_data);
  REQUIRE(res);
  source.close();
  REQUIRE(!source.is_open());

  REQUIRE(jpeg_reader.message_log().messages().empty());
  REQUIRE(img_data.width() == ref_width);
  REQUIRE(img_data.height() == ref_height);
  REQUIRE(img_data.stride_bytes() == ref_width * 3);
  REQUIRE(img_data.nr_channels() == 3);
  REQUIRE(img_data.nr_bytes_per_channel() == 1);
  REQUIRE(img_data.total_bytes() == img_data.stride_bytes() * img_data.height());
  REQUIRE(img_data.is_packed());
  REQUIRE(img_data.is_view());
  REQUIRE(!img_data.is_empty());
  REQUIRE(img_data.is_valid());
}

#endif  // defined(SELENE_WITH_LIBJPEG)
