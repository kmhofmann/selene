// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/selene_config.hpp>

#if defined(SELENE_WITH_LIBTIFF)

#include <catch2/catch.hpp>

#include <cstdlib>
#include <cstring>

#include <selene/base/io/FileReader.hpp>
#include <selene/base/io/FileUtils.hpp>
#include <selene/base/io/FileWriter.hpp>
#include <selene/base/io/MemoryReader.hpp>
#include <selene/base/io/VectorWriter.hpp>

#include <selene/img/dynamic/DynImage.hpp>
//#include <selene/img/dynamic/DynImageView.hpp>

//#include <selene/img/pixel/PixelTypeAliases.hpp>

//#include <selene/img/typed/Image.hpp>
//#include <selene/img/typed/ImageTypeAliases.hpp>
//#include <selene/img/typed/ImageView.hpp>

//#include <selene/img/interop/DynImageToImage.hpp>
//#include <selene/img/interop/ImageToDynImage.hpp>

#include <selene/img_io/tiff/Read.hpp>
#include <selene/img_io/tiff/Write.hpp>

#include <test/selene/Utils.hpp>

#include <wrappers/fs/Filesystem.hpp>

using namespace sln::literals;

constexpr auto stickers_ref_width = 400;
constexpr auto stickers_ref_height = 320;

namespace {

void check_write_read(const sln::DynImage& dyn_img,
                      const sln_fs::path& tmp_path,
                      sln::TIFFReadObject<sln::FileReader>& read_object,
                      sln::TIFFWriteObject<sln::FileWriter>& write_object)
{
  // Write as TIFF file...
  sln::FileWriter sink((tmp_path / "test_img.tif").string());
  REQUIRE(sink.is_open());
  sln::MessageLog messages_write;
  bool status_write = sln::write_tiff(dyn_img, sink, sln::TIFFWriteOptions(), &messages_write, &write_object);
  REQUIRE(status_write);
  REQUIRE(messages_write.messages().empty());
  sink.close();
  REQUIRE(!sink.is_open());

  // ...and read again
  sln::FileReader source((tmp_path / "test_img.tif").string());
  sln::MessageLog messages_read;
  auto dyn_img_2 = sln::read_tiff(source, &messages_read, &read_object);
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

  REQUIRE(sln::equal(dyn_img, dyn_img_2));
}

void check_test_suite(const sln_fs::path& test_suite_path,
                      const sln_fs::path& tmp_path,
                      const std::vector<std::string>& cannot_read_list,
                      const std::vector<std::string>& may_have_error_list)
{
  using sln_fs::directory_iterator;

  // Try to re-use the read and write objects.
  sln::TIFFReadObject<sln::FileReader> read_object;
  sln::TIFFWriteObject<sln::FileWriter> write_object;

  for (directory_iterator itr(test_suite_path), itr_end = directory_iterator(); itr != itr_end; ++itr)
  {
    const sln_fs::directory_entry& e = *itr;

    if (e.path().extension() == ".tif")
    {
      sln::FileReader source(e.path().string());
      REQUIRE(source.is_open());

      sln::MessageLog messages_read;
      auto dyn_imgs = sln::read_tiff_all(source, &messages_read, &read_object);

      const bool cannot_be_read = std::any_of(cannot_read_list.cbegin(), cannot_read_list.cend(),
                                              [&e](const auto& s) { return s == e.path().stem().string(); });

      if (cannot_be_read)
      {
        REQUIRE(!messages_read.messages().empty());
        REQUIRE(messages_read.contains_errors());
      }
      else
      {
        const bool may_have_error = std::any_of(may_have_error_list.cbegin(), may_have_error_list.cend(),
                                                [&e](const auto& s) { return s == e.path().stem().string(); });

        for (const auto& dyn_img : dyn_imgs)
        {
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

          check_write_read(dyn_img, tmp_path, read_object, write_object);
        }
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

TEST_CASE("TIFF image reading, through TIFFReader interface", "[img]")
{
  sln::FileReader source(sln_test::full_data_path("stickers_lzw.tif").string());
  REQUIRE(source.is_open());

  const auto pos = source.position();
  REQUIRE(source.position() == 0);

  sln::TIFFReader<sln::FileReader> tiff_reader;

  SECTION("With invalid source")
  {
    const auto layouts = tiff_reader.read_layouts();
    REQUIRE(layouts.empty());
    REQUIRE(!tiff_reader.advance_directory());
    REQUIRE(!tiff_reader.set_directory(0));
    REQUIRE(tiff_reader.read_image_data() == sln::DynImage{});
    sln::DynImage dyn_img;
    REQUIRE(!tiff_reader.read_image_data(dyn_img));
    REQUIRE(!tiff_reader.message_log().messages().empty());
  }

  SECTION("With valid source")
  {
    for (int i = 0; i < 2; ++i)
    {
      source.seek_abs(pos);
      REQUIRE(source.position() == 0);
      tiff_reader.set_source(source);

      const auto layouts = tiff_reader.read_layouts();
      REQUIRE(layouts.size() == 1);
      REQUIRE(layouts[0].width == stickers_ref_width);
      REQUIRE(layouts[0].height == stickers_ref_height);
      REQUIRE(layouts[0].samples_per_pixel == 3);
      REQUIRE(layouts[0].bits_per_sample == 8);

      sln::DynImage dyn_img({layouts[0].width_px(), layouts[0].height_px(),
                             layouts[0].nr_channels(), layouts[0].nr_bytes_per_channel()});
      auto res = tiff_reader.read_image_data(dyn_img);
      REQUIRE(res);

      REQUIRE(tiff_reader.message_log().messages().empty());
      REQUIRE(dyn_img.width() == stickers_ref_width);
      REQUIRE(dyn_img.height() == stickers_ref_height);
      REQUIRE(dyn_img.stride_bytes() == stickers_ref_width * 3);
      REQUIRE(dyn_img.nr_channels() == 3);
      REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
      REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
      REQUIRE(dyn_img.is_packed());
      REQUIRE(!dyn_img.is_empty());
      REQUIRE(dyn_img.is_valid());
    }
  }

  SECTION("Into view, successful")
  {
    source.seek_abs(pos);
    REQUIRE(source.position() == 0);
    tiff_reader.set_source(source);

    const auto layouts = tiff_reader.read_layouts();
    REQUIRE(layouts.size() == 1);

    sln::DynImage dyn_img({layouts[0].width_px(), layouts[0].height_px(),
                           layouts[0].nr_channels(), layouts[0].nr_bytes_per_channel()});
    sln::MutableDynImageView dyn_img_view{dyn_img.byte_ptr(), dyn_img.layout(), dyn_img.semantics()};
    auto res = tiff_reader.read_image_data(dyn_img_view);
    REQUIRE(res);

    REQUIRE(tiff_reader.message_log().messages().empty());
    REQUIRE(dyn_img.width() == stickers_ref_width);
    REQUIRE(dyn_img.height() == stickers_ref_height);
    REQUIRE(dyn_img.stride_bytes() == stickers_ref_width * 3);
    REQUIRE(dyn_img.nr_channels() == 3);
    REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
    REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
    REQUIRE(dyn_img.is_packed());
    REQUIRE(!dyn_img.is_empty());
    REQUIRE(dyn_img.is_valid());
  }

  SECTION("Into view, unsuccessful")
  {
    source.seek_abs(pos);
    tiff_reader.set_source(source);

    const auto layouts = tiff_reader.read_layouts();
    REQUIRE(layouts.size() == 1);

    sln::DynImage dyn_img({sln::to_pixel_length(layouts[0].width_px() + 1), layouts[0].height_px(),
                           layouts[0].nr_channels(), layouts[0].nr_bytes_per_channel()});
    sln::MutableDynImageView dyn_img_view{dyn_img.byte_ptr(), dyn_img.layout(), dyn_img.semantics()};
    auto res = tiff_reader.read_image_data(dyn_img_view);
    REQUIRE(!res);
  }

  source.close();
  REQUIRE(!source.is_open());
}

namespace {

template <typename SinkType, typename SourceType, typename SinkArg, typename SourceArgFunc>
void write_one_tiff_directory(const sln::DynImage& ref_img, SinkArg&& sink_arg, SourceArgFunc source_arg_func)
{
  SinkType sink{sink_arg};
  REQUIRE(sink.is_open());

  sln::TIFFWriter tiff_writer{sink};
  tiff_writer.write_image_data(ref_img);

  sink.close();
  REQUIRE(!sink.is_open());

  // See if we can read this image again.
  sln::MessageLog message_log_read;
  const auto dyn_imgs = sln::read_tiff_all(SourceType{source_arg_func()}, &message_log_read);
  REQUIRE(dyn_imgs.size() == 1);
  const auto& dyn_img = dyn_imgs[0];
  REQUIRE(message_log_read.messages().empty());
  REQUIRE(dyn_img.width() == stickers_ref_width);
  REQUIRE(dyn_img.height() == stickers_ref_height);
  REQUIRE(dyn_img.stride_bytes() == stickers_ref_width * 3);
  REQUIRE(dyn_img.nr_channels() == 3);
  REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
  REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
  REQUIRE(dyn_img.is_packed());
  REQUIRE(!dyn_img.is_empty());
  REQUIRE(dyn_img.is_valid());
}

template <typename SinkType, typename SourceType, typename SinkArg, typename SourceArgFunc>
void write_multiple_tiff_directories(const sln::DynImage& ref_img, SinkArg&& sink_arg, SourceArgFunc source_arg_func)
{
  for (std::size_t n = 1; n <= 4; ++n)
  {
    {
      SinkType sink{sink_arg};
      REQUIRE(sink.is_open());

      sln::TIFFWriter tiff_writer{sink};
      for (std::size_t i = 0; i < n; ++i)
      {
        tiff_writer.write_image_data(ref_img);
      }
      tiff_writer.finish_writing();
      REQUIRE(tiff_writer.message_log().messages().empty());

      sink.close();
      REQUIRE(!sink.is_open());
    }

    // See if we can read all written images again.
    {
      SourceType source{source_arg_func()};
      REQUIRE(source.is_open());
      sln::MessageLog message_log_read;
      const auto dyn_imgs = sln::read_tiff_all(source, &message_log_read);
      REQUIRE(message_log_read.messages().empty());
      REQUIRE(dyn_imgs.size() == n);

      for (const auto& dyn_img : dyn_imgs)
      {
        REQUIRE(dyn_img.width() == stickers_ref_width);
        REQUIRE(dyn_img.height() == stickers_ref_height);
        REQUIRE(dyn_img.stride_bytes() == stickers_ref_width * 3);
        REQUIRE(dyn_img.nr_channels() == 3);
        REQUIRE(dyn_img.nr_bytes_per_channel() == 1);
        REQUIRE(dyn_img.total_bytes() == dyn_img.stride_bytes() * dyn_img.height());
        REQUIRE(dyn_img.is_packed());
        REQUIRE(!dyn_img.is_empty());
        REQUIRE(dyn_img.is_valid());
      }
    }
  }
}

}  // namespace _

TEST_CASE("TIFF image writing, through TIFFWriter interface", "[img]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  sln::FileReader source(sln_test::full_data_path("stickers_lzw.tif").string());
  REQUIRE(source.is_open());
  const auto ref_img = sln::read_tiff(source);
  REQUIRE(ref_img.is_valid());
  REQUIRE(ref_img.width() == stickers_ref_width);
  REQUIRE(ref_img.height() == stickers_ref_height);
  source.close();

  const auto out_path = tmp_path / "test_img_out.tif";
  std::vector<std::uint8_t> out_vec;

  SECTION("Writing one TIFF directory, file I/O")
  {
    auto get_src_arg = [&out_path](){ return out_path.string(); };
    write_one_tiff_directory<sln::FileWriter, sln::FileReader>(ref_img, out_path.string(), get_src_arg);
  }

  SECTION("Writing multiple TIFF directories, file I/O")
  {
    auto get_src_arg = [&out_path](){ return out_path.string(); };
    write_multiple_tiff_directories<sln::FileWriter, sln::FileReader>(ref_img, out_path.string(), get_src_arg);
  }

  SECTION("Writing one TIFF directory, memory I/O")
  {
    auto get_src_arg = [&out_vec](){ return sln::ConstantMemoryRegion{out_vec.data(), out_vec.size()}; };
    write_one_tiff_directory<sln::VectorWriter, sln::MemoryReader>(ref_img, out_vec, get_src_arg);
  }

  SECTION("Writing multiple TIFF directories, memory I/O")
  {
    auto get_src_arg = [&out_vec](){ return sln::ConstantMemoryRegion{out_vec.data(), out_vec.size()}; };
    write_multiple_tiff_directories<sln::VectorWriter, sln::MemoryReader>(ref_img, out_vec, get_src_arg);
  }
}

#endif  // defined(SELENE_WITH_LIBTIFF)
