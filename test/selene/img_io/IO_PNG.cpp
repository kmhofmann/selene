// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBPNG)

#include <png.h>  // to check PNG_LIBPNG_VER_MAJOR and PNG_LIBPNG_VER_MINOR below

#include <catch2/catch.hpp>

#include <cstdlib>
#include <cstring>

#include <boost/filesystem.hpp>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileUtils.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/MemoryReader.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/dynamic/DynImage.hpp>
#include <selene/img/dynamic/DynImageView.hpp>

#include <selene/img/pixel/PixelTypeAliases.hpp>

#include <selene/img/typed/Image.hpp>
#include <selene/img/typed/ImageTypeAliases.hpp>
#include <selene/img/typed/ImageView.hpp>

#include <selene/img/interop/DynImageToImage.hpp>
#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/PNGRead.hpp>
#include <selene/img_io/PNGWrite.hpp>

#include <test/selene/Utils.hpp>

namespace fs = boost::filesystem;
using namespace sln::literals;

constexpr auto ref_width = 1024;
constexpr auto ref_height = 684;
constexpr std::array<std::array<unsigned int, 6>, 3> pix = {
    {std::array<unsigned int, 6>{{226, 180, 244, 198, 0, 203}},
     std::array<unsigned int, 6>{{582, 415, 228, 227, 233, 227}},
     std::array<unsigned int, 6>{{878, 597, 57, 60, 69, 61}}}};  // {x, y}, {r, g, b}, {y}

namespace {

fs::path test_suite_dir()
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? fs::path(env_var) / "png_suite" : fs::path("../data/png_suite");
}

void check_write_read(sln::DynImage& dyn_img, const fs::path& tmp_path)
{
  // Write as PNG file...
  sln::FileWriter sink((tmp_path / "test_img.png").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_png(dyn_img, sink, sln::PNGCompressionOptions(), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
  sink.close();
  REQUIRE(!sink.is_open());

  // ...and read again
  sln::FileReader source((tmp_path / "test_img.png").string());
  sln::MessageLog messages_read;
  auto dyn_img_2 = sln::read_png(source, sln::PNGDecompressionOptions(), &messages_read);
  REQUIRE(messages_read.messages().empty());
  source.close();
  REQUIRE(!source.is_open());

  // Make sure that content and state is the same
  REQUIRE(dyn_img_2.is_valid());
  REQUIRE(dyn_img_2.width() == dyn_img.width());
  REQUIRE(dyn_img_2.height() == dyn_img.height());
  REQUIRE(dyn_img_2.stride_bytes() == dyn_img.stride_bytes());
  REQUIRE(dyn_img_2.nr_channels() == dyn_img.nr_channels());
  REQUIRE(dyn_img_2.nr_bytes_per_channel() == dyn_img.nr_bytes_per_channel());
  REQUIRE(dyn_img_2.pixel_format() == dyn_img.pixel_format());
  REQUIRE(dyn_img_2.sample_format() == dyn_img.sample_format());
  REQUIRE(dyn_img_2.total_bytes() == dyn_img.total_bytes());

  const auto nr_bytes_per_row = dyn_img_2.width() * dyn_img_2.nr_channels() * dyn_img_2.nr_bytes_per_channel();
  for (auto y = 0_idx; y < dyn_img_2.height(); ++y)
  {
    REQUIRE(std::memcmp(dyn_img_2.byte_ptr(y), dyn_img.byte_ptr(y), static_cast<std::size_t>(nr_bytes_per_row)) == 0);
  }
}

}  // namespace

TEST_CASE("PNG image reading and writing, no conversion", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  sln::FileReader source(sln_test::full_data_path("bike_duck.png").string());
  REQUIRE(source.is_open());

  // Test reading without conversion
  sln::MessageLog messages_read;
  auto dyn_img = sln::read_png(source, sln::PNGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(dyn_img.width() == ref_width);
  REQUIRE(dyn_img.height() == ref_height);
  REQUIRE(dyn_img.stride_bytes() == ref_width * 3);
  REQUIRE(dyn_img.nr_channels() == 3);
  REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
  REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
  REQUIRE(dyn_img.is_packed());
  REQUIRE(!dyn_img.is_empty());
  REQUIRE(dyn_img.is_valid());

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(dyn_img));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (std::size_t i = 0; i < 3; ++i)
  {
    const auto x = sln::to_pixel_index(pix[i][0]);
    const auto y = sln::to_pixel_index(pix[i][1]);
    REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
  }

  sln::FileWriter sink((tmp_path / "test_duck.png").string());
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  sln::MessageLog messages_write;
  bool status_write = sln::write_png(to_dyn_image_view(img, sln::PixelFormat::RGB), sink, sln::PNGCompressionOptions(),
                                     &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG image reading and writing, conversion to grayscale", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  sln::FileReader source(sln_test::full_data_path("bike_duck.png").string());
  REQUIRE(source.is_open());

  // Test reading with conversion to grayscale
  sln::PNGDecompressionOptions decompression_options;
  decompression_options.convert_rgb_to_gray = true;
  sln::MessageLog messages_read;
  auto dyn_img = sln::read_png(source, decompression_options, &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(dyn_img.width() == ref_width);
  REQUIRE(dyn_img.height() == ref_height);
  REQUIRE(dyn_img.stride_bytes() == ref_width * 1);
  REQUIRE(dyn_img.nr_channels() == 1);
  REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
  REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
  REQUIRE(dyn_img.is_packed());
  REQUIRE(!dyn_img.is_empty());
  REQUIRE(dyn_img.is_valid());

  auto img = sln::to_image<sln::Pixel_8u1>(std::move(dyn_img));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 1);
  for (std::size_t i = 0; i < 3; ++i)
  {
    const auto x = sln::to_pixel_index(pix[i][0]);
    const auto y = sln::to_pixel_index(pix[i][1]);
#if (PNG_LIBPNG_VER_MAJOR > 1) || (PNG_LIBPNG_VER_MAJOR == 1 && PNG_LIBPNG_VER_MINOR >= 6)
    REQUIRE(static_cast<int>(img(x, y)) == static_cast<int>(sln::Pixel_8u1(pix[i][5])));
#endif
  }

  sln::FileWriter sink((tmp_path / "test_duck_gray.png").string());
  REQUIRE(sink.is_open());

  // Test writing of grayscale image
  sln::MessageLog messages_write;
  bool status_write = sln::write_png(to_dyn_image_view(img, sln::PixelFormat::Y), sink, sln::PNGCompressionOptions(),
                                     &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG image reading, reusing decompression object", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  sln::PNGDecompressionObject decompression_object;

  for (int j = 0; j < 5; ++j)
  {
    sln::FileReader source(sln_test::full_data_path("bike_duck.png").string());
    REQUIRE(source.is_open());

    // Test reading of header...
    const auto header = read_png_header(decompression_object, source);
    REQUIRE(header.width == ref_width);
    REQUIRE(header.height == ref_height);
    REQUIRE(header.nr_channels == 3);
    REQUIRE(header.bit_depth == 8);

    // ...and then reusing decompression object/header info for reading the image
    sln::MessageLog messages_read;
    auto dyn_img = sln::read_png(decompression_object, source, sln::PNGDecompressionOptions(), &messages_read, &header);

    REQUIRE(messages_read.messages().empty());
    REQUIRE(dyn_img.width() == ref_width);
    REQUIRE(dyn_img.height() == ref_height);
    REQUIRE(dyn_img.stride_bytes() == ref_width * 3);
    REQUIRE(dyn_img.nr_channels() == 3);
    REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
    REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
    REQUIRE(dyn_img.is_packed());
    REQUIRE(!dyn_img.is_empty());
    REQUIRE(dyn_img.is_valid());

    auto img = sln::to_image<sln::Pixel_8u3>(std::move(dyn_img));

    REQUIRE(img.width() == ref_width);
    REQUIRE(img.height() == ref_height);
    REQUIRE(img.stride_bytes() == ref_width * 3);
    for (std::size_t i = 0; i < 3; ++i)
    {
      const auto x = sln::to_pixel_index(pix[i][0]);
      const auto y = sln::to_pixel_index(pix[i][1]);
      REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
    }
  }
}

TEST_CASE("PNG image writing, reusing compression object", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  // First, read an image
  sln::MessageLog message_log_read;
  auto dyn_img = sln::read_png(sln::FileReader(sln_test::full_data_path("bike_duck.png").string()),
                               sln::PNGDecompressionOptions(),
                               &message_log_read);
  REQUIRE(dyn_img.is_valid());
  REQUIRE(message_log_read.messages().empty());

  sln::PNGCompressionObject comp_obj;

  for (int i = 0; i < 5; ++i)
  {
    // Test writing of grayscale image
    sln::FileWriter sink((tmp_path / "test_duck_gray.png").string());
    REQUIRE(sink.is_open());
    sln::MessageLog messages_write;
    bool status_write = sln::write_png(dyn_img, comp_obj, sink, sln::PNGCompressionOptions(), &messages_write);
    REQUIRE(status_write);
    REQUIRE(messages_write.messages().empty());
  }
}

TEST_CASE("PNG image reading and writing, reading/writing from/to memory", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto file_contents = sln::read_file_contents(sln_test::full_data_path("bike_duck.png").string());
  REQUIRE(!file_contents.empty());

  // Test reading from memory
  sln::MemoryReader source(file_contents.data(), file_contents.size());
  REQUIRE(source.is_open());

  // Test reading without conversion
  sln::MessageLog messages_read;
  auto dyn_img = sln::read_png(source, sln::PNGDecompressionOptions(), &messages_read);

  REQUIRE(messages_read.messages().empty());
  REQUIRE(dyn_img.width() == ref_width);
  REQUIRE(dyn_img.height() == ref_height);
  REQUIRE(dyn_img.stride_bytes() == ref_width * 3);
  REQUIRE(dyn_img.nr_channels() == 3);
  REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
  REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
  REQUIRE(dyn_img.is_packed());
  REQUIRE(!dyn_img.is_empty());
  REQUIRE(dyn_img.is_valid());

  auto img = sln::to_image<sln::Pixel_8u3>(std::move(dyn_img));

  REQUIRE(img.width() == ref_width);
  REQUIRE(img.height() == ref_height);
  REQUIRE(img.stride_bytes() == ref_width * 3);
  for (std::size_t i = 0; i < 3; ++i)
  {
    const auto x = sln::to_pixel_index(pix[i][0]);
    const auto y = sln::to_pixel_index(pix[i][1]);
    REQUIRE(img(x, y) == sln::Pixel_8u3(pix[i][2], pix[i][3], pix[i][4]));
  }

  // Test writing to memory
  std::vector<std::uint8_t> compressed_data;
  sln::VectorWriter sink(compressed_data);
  REQUIRE(sink.is_open());

  // Test writing of RGB image
  sln::MessageLog messages_write;
  bool status_write = sln::write_png(to_dyn_image_view(img, sln::PixelFormat::RGB), sink, sln::PNGCompressionOptions(),
                                     &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

TEST_CASE("PNG reading of the official test suite", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto test_suite_path = test_suite_dir();

  using fs::directory_iterator;
  sln::PNGDecompressionObject dec_obj;

  for (directory_iterator itr(test_suite_path), itr_end = directory_iterator(); itr != itr_end; ++itr)
  {
    const fs::directory_entry& e = *itr;

    if (e.path().extension() == ".png")
    {
      sln::FileReader source(e.path().string());
      REQUIRE(source.is_open());

      sln::MessageLog messages_read;
      auto dyn_img = sln::read_png(dec_obj, source, sln::PNGDecompressionOptions(), &messages_read);

      const auto filename_first_letter = e.path().stem().c_str()[0];
      const auto is_broken = (filename_first_letter == 'x');  // Broken image files begin with 'x'

      if (!is_broken)
      {
        // REQUIRE(messages_read.messages().empty());  // Might not be guaranteed, so this is disabled
        REQUIRE(dyn_img.width() > 0);
        REQUIRE(dyn_img.height() > 0);
        REQUIRE(dyn_img.stride_bytes() > 0);
        REQUIRE(dyn_img.nr_channels() > 0);
        REQUIRE(dyn_img.nr_bytes_per_channel() > 0);
        REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
        REQUIRE(dyn_img.is_packed());
        REQUIRE(!dyn_img.is_empty());
        REQUIRE(dyn_img.is_valid());

        check_write_read(dyn_img, tmp_path);
      }
      else
      {
        REQUIRE(!messages_read.messages().empty());
        REQUIRE(!dyn_img.is_valid());
      }
    }
  }
}

TEST_CASE("PNG image reading, through PNGReader interface", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  sln::FileReader source(sln_test::full_data_path("bike_duck.png").string());
  REQUIRE(source.is_open());
  const auto pos = source.position();

  sln::PNGReader<sln::FileReader> png_reader;

  {
    const auto header = png_reader.read_header();
    REQUIRE(!header.is_valid());
    const auto info = png_reader.get_output_image_info();
    REQUIRE(!info.is_valid());
    sln::DynImage dyn_img;
    const auto res = png_reader.read_image_data(dyn_img);
    REQUIRE(!res);
  }

  for (int i = 0; i < 5; ++i)
  {
    source.seek_abs(pos);
    png_reader.set_source(source);

    const auto header = png_reader.read_header();
    REQUIRE(header.is_valid());
    REQUIRE(header.width == ref_width);
    REQUIRE(header.height == ref_height);
    REQUIRE(header.nr_channels == 3);
    REQUIRE(header.bit_depth == 8);

    png_reader.set_decompression_options(sln::PNGDecompressionOptions());
    const auto info = png_reader.get_output_image_info();
    REQUIRE(info.is_valid());
    REQUIRE(info.width == ref_width);
    REQUIRE(info.height == ref_height);
    REQUIRE(info.nr_channels == 3);
    REQUIRE(info.bit_depth == 8);

    sln::DynImage dyn_img({info.width, info.height, info.nr_channels, info.nr_bytes_per_channel()});
    auto res = png_reader.read_image_data(dyn_img);
    REQUIRE(res);

    REQUIRE(png_reader.message_log().messages().empty());
    REQUIRE(dyn_img.width() == ref_width);
    REQUIRE(dyn_img.height() == ref_height);
    REQUIRE(dyn_img.stride_bytes() == ref_width * 3);
    REQUIRE(dyn_img.nr_channels() == 3);
    REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
    REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
    REQUIRE(dyn_img.is_packed());
    REQUIRE(!dyn_img.is_empty());
    REQUIRE(dyn_img.is_valid());
  }

  source.close();
  REQUIRE(!source.is_open());
}


#endif  // defined(SELENE_WITH_LIBPNG)
