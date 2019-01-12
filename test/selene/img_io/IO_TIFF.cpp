// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#if defined(SELENE_WITH_LIBTIFF)

#include <catch2/catch.hpp>

#include <cstdlib>
#include <cstring>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileUtils.hpp>
#include <selene/base/io/FileWriter.hpp>
//#include <selene/base/io/MemoryReader.hpp>
//#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/dynamic/DynImage.hpp>
//#include <selene/img/dynamic/DynImageView.hpp>

//#include <selene/img/pixel/PixelTypeAliases.hpp>

//#include <selene/img/typed/Image.hpp>
//#include <selene/img/typed/ImageTypeAliases.hpp>
//#include <selene/img/typed/ImageView.hpp>

//#include <selene/img/interop/DynImageToImage.hpp>
//#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/TIFFRead.hpp>
#include <selene/img_io/TIFFWrite.hpp>

#include <test/selene/Utils.hpp>

#include <wrappers/fs/Filesystem.hpp>

using namespace sln::literals;

namespace {

void check_write_read(sln::DynImage& dyn_img, const sln_fs::path& tmp_path)
{
  // Write as TIFF file...
  sln::FileWriter sink((tmp_path / "test_img.tif").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_tiff(dyn_img, sink, sln::TIFFWriteOptions(), &messages_write);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
  sink.close();
  REQUIRE(!sink.is_open());

  // ...and read again
  sln::FileReader source((tmp_path / "test_img.tif").string());
  sln::MessageLog messages_read;
  auto dyn_img_2 = sln::read_tiff(source, &messages_read);
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

void check_test_suite(const sln_fs::path& test_suite_path,
                      const sln_fs::path& tmp_path,
                      const std::vector<std::string>& cannot_read_list,
                      const std::vector<std::string>& may_have_error_list)
{
  using sln_fs::directory_iterator;

  for (directory_iterator itr(test_suite_path), itr_end = directory_iterator(); itr != itr_end; ++itr)
  {
    const sln_fs::directory_entry& e = *itr;

    if (e.path().extension() == ".tif")
    {
      sln::FileReader source(e.path().string());
      REQUIRE(source.is_open());

      sln::MessageLog messages_read;
      sln::TIFFReadObject<sln::FileReader> read_object;
      auto dyn_img = sln::read_tiff(source, &messages_read, &read_object);

      const bool cannot_be_read = std::any_of(cannot_read_list.cbegin(), cannot_read_list.cend(),
                                              [&e](const auto& s) { return s == e.path().stem().string(); });

      if (cannot_be_read)
      {
        REQUIRE(!messages_read.messages().empty());
        REQUIRE(messages_read.contains_errors());
//        REQUIRE(!dyn_img.is_valid());
      }
      else
      {
        const bool may_have_error = std::any_of(may_have_error_list.cbegin(), may_have_error_list.cend(),
                                                [&e](const auto& s) { return s == e.path().stem().string(); });
        REQUIRE(static_cast<bool>(!messages_read.contains_errors() || may_have_error));
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
    }
  }
}

}  // namespace


// TODO: more extensive tests for reading & writing TIFF images


TEST_CASE("TIFF reading of the official test suite", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto test_suite_path = sln_test::full_data_path("libtiffpic");

  const std::vector<std::string> cannot_read_list = {
      "caspian", // bit depth 64
      "jello", // palette
      "off_l16", // photometric: logl
      "off_luv24", // photometric: logluv
      "off_luv32", // photometric: logluv
  };

  const std::vector<std::string> may_have_error_list = {
      "text"
  };

  check_test_suite(test_suite_path, tmp_path, cannot_read_list, may_have_error_list);
}

TEST_CASE("TIFF reading of the self-produced test suite", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto test_suite_path = sln_test::full_data_path("tiff_test");

  check_test_suite(test_suite_path, tmp_path, {}, {});
}

#endif  // defined(SELENE_WITH_LIBTIFF)
