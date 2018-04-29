// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>
#include <selene/io/FileReader.hpp>
#include <selene/io/FileUtils.hpp>
#include <selene/io/FileWriter.hpp>
#include <selene/io/MemoryReader.hpp>
#include <selene/io/MemoryWriter.hpp>
#include <selene/io/VectorReader.hpp>
#include <selene/io/VectorWriter.hpp>

#include <Utils.hpp>

#include <algorithm>
#include <cstring>
#include <limits>
#include <random>

bool handle_valid(sln::FileReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(sln::FileWriter& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(sln::MemoryReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(sln::MemoryWriter& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(sln::VectorReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(sln::VectorWriter& source)
{
  return source.handle() != nullptr;
}

template <typename SinkType, typename Target>
void write_test_1(Target* target)
{
  SinkType f;
  REQUIRE(!handle_valid(f));
  REQUIRE(!f.is_open());
  REQUIRE(f.is_eof());
  REQUIRE(f.position() == -1);

  f.open(*target, sln::WriterMode::Write);
  REQUIRE(handle_valid(f));
  REQUIRE(f.is_open());
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 0);

  write(f, std::uint8_t{'x'});
  write(f, std::int32_t{42});
  write(f, sln::float64_t{3.14});
  REQUIRE(f.position() == 13);

  f.seek_rel(-8);
  REQUIRE(f.position() == 5);
  write(f, sln::float64_t(6.28));
  REQUIRE(f.position() == 13);
  f.seek_abs(1);
  REQUIRE(f.position() == 1);
  write(f, std::int32_t{128});

  f.close();
  REQUIRE(!handle_valid(f));
  REQUIRE(!f.is_open());
  REQUIRE(f.is_eof());
  REQUIRE(f.position() == -1);
}

template <typename SourceType, typename Source>
void read_test_1(Source* source)
{
  SourceType f;
  REQUIRE(!handle_valid(f));
  REQUIRE(!f.is_open());
  REQUIRE(f.is_eof());
  REQUIRE(f.position() == -1);

  f.open(*source);
  REQUIRE(handle_valid(f));
  REQUIRE(f.is_open());
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 0);

  REQUIRE(sln::read<std::uint8_t>(f) == 'x');
  REQUIRE(sln::read<std::int32_t>(f) == 128);
  REQUIRE(sln::read<sln::float64_t>(f) == 6.28);
}

template <typename SinkType, typename Target>
void write_test_2(Target* target)
{
  SinkType f(*target, sln::WriterMode::Append);
  REQUIRE(handle_valid(f));
  REQUIRE(f.is_open());
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 13);

  write(f, std::int16_t{1000});
  write(f, std::uint8_t{255});
  REQUIRE(f.position() == 16);

  REQUIRE(f.seek_rel(-11));
  REQUIRE(f.position() == 5);
  write(f, std::uint32_t{43});
  REQUIRE(f.position() == 9);
}

template <typename SourceType, typename Source>
void read_test_2(Source* source)
{
  SourceType f(*source);
  REQUIRE(f.position() == 0);

  f.seek_abs(13);
  REQUIRE(sln::read<std::int16_t>(f) == 1000);
  REQUIRE(sln::read<std::uint8_t>(f) == 255);
  REQUIRE(f.position() == 16);

  std::uint8_t tmp;
  bool rc = sln::read<std::uint8_t>(f, tmp);
  REQUIRE(!rc);
  REQUIRE(f.is_eof());

  f.seek_abs(5);
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 5);
  REQUIRE(sln::read<std::uint32_t>(f) == 43);
  REQUIRE(f.position() == 9);

  f.rewind();
  REQUIRE(f.position() == 0);
  REQUIRE(!f.is_eof());
}

TEST_CASE("Test I/O classes", "[io]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto filename = tmp_path / "test_io.bin";

  const auto& filename_str = filename.string();
  write_test_1<sln::FileWriter>(&filename_str);
  read_test_1<sln::FileReader>(&filename_str);
  write_test_2<sln::FileWriter>(&filename_str);
  read_test_2<sln::FileReader>(&filename_str);

  std::vector<std::uint8_t> vec;
  write_test_1<sln::VectorWriter>(&vec);
  read_test_1<sln::VectorReader>(&vec);
  write_test_2<sln::VectorWriter>(&vec);
  read_test_2<sln::VectorReader>(&vec);
}

TEST_CASE("Test binary data I/O", "[io]")
{
  const auto tmp_path = sln_test::get_tmp_path();
  const auto filename = tmp_path / "test_file_utils.bin";

  SECTION("From string")
  {
    const std::string data_str =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor "
        "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco "
        "laboris "
        "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse "
        "cillum "
        "dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia "
        "deserunt mollit anim id est laborum.";

    sln::write_data_contents(filename.string(), data_str.data(), data_str.size());
    const std::vector<std::uint8_t> data_read = sln::read_file_contents(filename.string());
    REQUIRE(data_read.size() == data_str.size());
    REQUIRE(std::memcmp(data_str.data(), data_read.data(), data_str.size()) == 0);
  }

  SECTION("From random data")
  {
    std::mt19937 rng(12);
    std::uniform_int_distribution<std::int16_t> dist_i8(std::numeric_limits<std::int8_t>::min(),
                                                        std::numeric_limits<std::int8_t>::max());
    std::uniform_int_distribution<std::int16_t> dist_u8(std::numeric_limits<std::uint8_t>::min(),
                                                        std::numeric_limits<std::uint8_t>::max());

    for (std::size_t i = 1; i < 20; ++i)
    {
      const auto len = i * 1024;

      std::vector<std::int8_t> data_vec_i(len);
      std::generate(std::begin(data_vec_i), std::end(data_vec_i),
                    [&]() { return static_cast<std::int8_t>(dist_i8(rng)); });

      sln::write_data_contents(filename.string(), data_vec_i.data(), data_vec_i.size());
      const std::vector<std::uint8_t> data_read_i = sln::read_file_contents(filename.string());
      REQUIRE(data_vec_i.size() == len);
      REQUIRE(data_read_i.size() == len);
      REQUIRE(std::memcmp(data_vec_i.data(), data_read_i.data(), data_read_i.size()) == 0);

      std::vector<std::uint8_t> data_vec_u(len);
      std::generate(std::begin(data_vec_u), std::end(data_vec_u),
                    [&]() { return static_cast<std::uint8_t>(dist_u8(rng)); });

      sln::write_data_contents(filename.string(), data_vec_u.data(), data_vec_u.size());
      const std::vector<std::uint8_t> data_read_u = sln::read_file_contents(filename.string());
      REQUIRE(data_vec_u.size() == len);
      REQUIRE(data_read_u.size() == len);
      REQUIRE(std::memcmp(data_vec_u.data(), data_read_u.data(), data_read_u.size()) == 0);
    }
  }
}