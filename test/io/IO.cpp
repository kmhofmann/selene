// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <catch.hpp>

#include <selene/base/Types.hpp>
#include <selene/io/FileReader.hpp>
#include <selene/io/FileWriter.hpp>
#include <selene/io/MemoryReader.hpp>
#include <selene/io/MemoryWriter.hpp>
#include <selene/io/VectorReader.hpp>
#include <selene/io/VectorWriter.hpp>

#include <Utils.hpp>

using namespace selene;
using namespace selene::io;

bool handle_valid(FileReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(FileWriter& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(MemoryReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(MemoryWriter& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(VectorReader& source)
{
  return source.handle() != nullptr;
}

bool handle_valid(VectorWriter& source)
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

  f.open(*target, WriterMode::Write);
  REQUIRE(handle_valid(f));
  REQUIRE(f.is_open());
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 0);

  write(f, std::uint8_t{'x'});
  write(f, std::int32_t{42});
  write(f, float64_t{3.14});
  REQUIRE(f.position() == 13);

  f.seek_rel(-8);
  REQUIRE(f.position() == 5);
  write(f, float64_t(6.28));
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

  REQUIRE(read<std::uint8_t>(f) == 'x');
  REQUIRE(read<std::int32_t>(f) == 128);
  REQUIRE(read<float64_t>(f) == 6.28);
}

template <typename SinkType, typename Target>
void write_test_2(Target* target)
{
  SinkType f(*target, WriterMode::Append);
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
  REQUIRE(read<std::int16_t>(f) == 1000);
  REQUIRE(read<std::uint8_t>(f) == 255);
  REQUIRE(f.position() == 16);

  std::uint8_t tmp;
  bool rc = read<std::uint8_t>(f, tmp);
  REQUIRE(!rc);
  REQUIRE(f.is_eof());

  f.seek_abs(5);
  REQUIRE(!f.is_eof());
  REQUIRE(f.position() == 5);
  REQUIRE(read<std::uint32_t>(f) == 43);
  REQUIRE(f.position() == 9);

  f.rewind();
  REQUIRE(f.position() == 0);
  REQUIRE(!f.is_eof());
}

TEST_CASE("IO")
{
  const auto tmp_path = get_tmp_path();
  const auto filename = tmp_path / "test_io.bin";

  const char* c_filename = filename.c_str();
  write_test_1<FileWriter>(&c_filename);
  read_test_1<FileReader>(&c_filename);
  write_test_2<FileWriter>(&c_filename);
  read_test_2<FileReader>(&c_filename);

  std::vector<std::uint8_t> mem(100);
  write_test_1<VectorWriter>(&mem);
  read_test_1<VectorReader>(&mem);
  write_test_2<VectorWriter>(&mem);
  read_test_2<VectorReader>(&mem);

  std::vector<std::uint8_t> vec;
  write_test_1<VectorWriter>(&vec);
  read_test_1<VectorReader>(&vec);
  write_test_2<VectorWriter>(&vec);
  read_test_2<VectorReader>(&vec);
}
