// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <boost/filesystem.hpp>

#include <selene/base/MessageLog.hpp>

#include <selene/img/IO.hpp>

#include <selene/io/FileReader.hpp>
#include <selene/io/FileWriter.hpp>
#include <selene/io/VectorWriter.hpp>

#include <Utils.hpp>

namespace fs = boost::filesystem;

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
  const auto tmp_path = get_tmp_path();

  {
    // Read a JPEG file
    const auto img_path = full_path("bike_duck.jpg");
    sln::FileReader source(img_path.string());
    REQUIRE(source.is_open());

    sln::MessageLog messages_read;
    const auto img_data = sln::read_image(source, &messages_read);

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

    // Write as PNG
    sln::FileWriter sink((tmp_path / "test_duck_auto.png").string());
    REQUIRE(sink.is_open());

    sln::MessageLog messages_write;
    bool status_write = sln::write_image(img_data, sln::ImageFormat::PNG, sink, &messages_write);

    REQUIRE(status_write);
    REQUIRE(messages_write.messages().empty());
  }

  {
    // Read the previously written PNG image
    sln::FileReader source((tmp_path / "test_duck_auto.png").string());
    REQUIRE(source.is_open());

    sln::MessageLog messages;
    const auto img_data = sln::read_image(source, &messages);

    REQUIRE(messages.messages().empty());
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

    // Write as JPEG, to memory
    std::vector<std::uint8_t> png_data;
    sln::VectorWriter sink(png_data);

    sln::MessageLog messages_write;
    bool status_write = write_image(img_data, sln::ImageFormat::JPEG, sink, &messages_write);

    REQUIRE(status_write);
    REQUIRE(messages_write.messages().empty());
  }
}
