// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch2/catch.hpp>

#include <selene/base/MessageLog.hpp>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img_io/IO.hpp>

#include <test/selene/Utils.hpp>

constexpr auto duck_ref_width = 1024;
constexpr auto duck_ref_height = 684;

constexpr auto stickers_ref_width = 400;
constexpr auto stickers_ref_height = 320;

namespace {

template <typename Source>
auto read_img(Source&& source, std::int32_t ref_width, std::int32_t ref_height)
{
  REQUIRE(source.is_open());

  sln::MessageLog messages_read;
  const auto dyn_img = sln::read_image(std::forward<Source>(source), &messages_read);

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

  REQUIRE(dyn_img.pixel_format() == sln::PixelFormat::RGB);
  REQUIRE(dyn_img.sample_format() == sln::SampleFormat::UnsignedInteger);

  return dyn_img;
}

template <typename Sink>
void write_img(const sln::DynImage& dyn_img, sln::ImageFormat format, Sink&& sink)
{
  REQUIRE(sink.is_open());

  sln::MessageLog messages_write;
  bool status_write = write_image(dyn_img, format, sink, &messages_write);

  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
}

}  // namespace _

TEST_CASE("Image reading with automatic format selection", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

#if defined(SELENE_WITH_LIBJPEG)
  SECTION("Read a JPEG file, and write as a JPEG file")
  {
    const auto img_path = sln_test::full_data_path("bike_duck.jpg");

    // Read a JPEG image
    sln::FileReader source(img_path.string());
    auto dyn_img = read_img(source, duck_ref_width, duck_ref_height);

    // Write as JPEG
    sln::FileWriter sink((tmp_path / "test_duck_auto.jpg").string());
    write_img(dyn_img, sln::ImageFormat::JPEG, sink);

    // Write as JPEG to memory
    std::vector<std::uint8_t> vec;
    sln::VectorWriter sink_mem(vec);
    write_img(dyn_img, sln::ImageFormat::JPEG, sink_mem);
  }
#endif // defined(SELENE_WITH_LIBJPEG)

#if defined(SELENE_WITH_LIBPNG)
  SECTION("Read a PNG file, and write as a PNG file")
  {
    const auto img_path = sln_test::full_data_path("bike_duck.png");

    // Read a PNG image
    sln::FileReader source(img_path.string());
    auto dyn_img = read_img(source, duck_ref_width, duck_ref_height);

    // Write as PNG
    sln::FileWriter sink((tmp_path / "test_duck_auto.png").string());
    write_img(dyn_img, sln::ImageFormat::PNG, sink);

    // Write as PNG to memory
    std::vector<std::uint8_t> vec;
    sln::VectorWriter sink_mem(vec);
    write_img(dyn_img, sln::ImageFormat::PNG, sink_mem);
  }
#endif // defined(SELENE_WITH_LIBPNG)

#if defined(SELENE_WITH_LIBTIFF)
  SECTION("Read a TIFF file, and write as a TIFF file")
  {
    const auto img_path = sln_test::full_data_path("stickers_jpeg.tif");

    // Read a TIFF image
    sln::FileReader source(img_path.string());
    auto dyn_img = read_img(source, stickers_ref_width, stickers_ref_height);

    // Write as TIFF
    sln::FileWriter sink((tmp_path / "test_stickers_auto.tif").string());
    write_img(dyn_img, sln::ImageFormat::TIFF, sink);

    // Write as TIFF to memory
    std::vector<std::uint8_t> vec;
    sln::VectorWriter sink_mem(vec);
    write_img(dyn_img, sln::ImageFormat::TIFF, sink_mem);
  }
#endif // defined(SELENE_WITH_LIBTIFF)
}
