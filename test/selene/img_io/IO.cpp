// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/MessageLog.hpp>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img_io/IO.hpp>

#include <test/selene/Utils.hpp>

#include <filesystem>

namespace fs = std::filesystem;

constexpr auto ref_width = 1024;
constexpr auto ref_height = 684;

namespace {

fs::path full_path(const char* filename)
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  return (env_var) ? (fs::path(env_var) / fs::path(filename)) : (fs::path("../data") / fs::path(filename));
}

}  // namespace

TEST_CASE("Image reading with automatic format selection", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();

  SECTION("Read a JPEG file, and write as PNG")
  {
    // Read a JPEG file
    const auto img_path = full_path("bike_duck.jpg");
    sln::FileReader source(img_path.string());
    REQUIRE(source.is_open());

    sln::MessageLog messages_read;
    const auto dyn_img = sln::read_image(source, &messages_read);

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

    // Write as PNG
    sln::FileWriter sink((tmp_path / "test_duck_auto.png").string());
    REQUIRE(sink.is_open());

    sln::MessageLog messages_write;
    bool status_write = sln::write_image(dyn_img, sln::ImageFormat::PNG, sink, &messages_write);

    REQUIRE(status_write);
    REQUIRE(messages_write.messages().empty());
  }

  SECTION("Read the previously written PNG image; then write as JPEG to memory")
  {
    // Read the previously written PNG image
    sln::FileReader source((tmp_path / "test_duck_auto.png").string());
    REQUIRE(source.is_open());

    sln::MessageLog messages;
    const auto dyn_img = sln::read_image(source, &messages);

    REQUIRE(messages.messages().empty());
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

    // Write as JPEG, to memory
    std::vector<std::uint8_t> png_data;
    sln::VectorWriter sink(png_data);

    sln::MessageLog messages_write;
    bool status_write = write_image(dyn_img, sln::ImageFormat::JPEG, sink, &messages_write);

    REQUIRE(status_write);
    REQUIRE(messages_write.messages().empty());
  }
}
