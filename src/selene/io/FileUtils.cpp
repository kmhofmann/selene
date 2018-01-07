// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/io/FileUtils.hpp>

#include <array>
#include <cstdio>
#include <sstream>

namespace sln {

/** \brief Reads the binary contents of a file and returns them.
 *
 * If the file can not be opened or read, the function will throw a `std::runtime_error` exception.
 *
 * \param path A string representing the path to the file that should be read.
 * \return A `std::vector<std::uint8_t>` with the contents of the file.
 */
std::vector<std::uint8_t> read_file_contents(const std::string& path)
{
  constexpr std::size_t buffer_size = 32768;
  std::array<std::uint8_t, buffer_size> buffer{};
  auto fp = std::fopen(path.c_str(), "rb");

  if (fp == nullptr)
  {
    std::stringstream ss;
    ss << "Cannot open file " << path << " for reading";
    throw std::runtime_error(ss.str());
  }

  std::vector<std::uint8_t> contents;
  while (!std::feof(fp))
  {
    auto nr_read = std::fread(buffer.data(), sizeof(std::uint8_t), buffer_size, fp);
    contents.insert(std::end(contents), buffer.data(), buffer.data() + nr_read);
  }

  std::fclose(fp);
  return contents;
}

void write_data_contents(const std::string& path, const std::vector<std::uint8_t>& data)
{
  auto fp = std::fopen(path.c_str(), "wb");

  if (fp == nullptr)
  {
    std::stringstream ss;
    ss << "Cannot open file " << path << " for writing";
    throw std::runtime_error(ss.str());
  }

  constexpr std::size_t chunk_size = 32768;
  const std::size_t nr_full_chunks = data.size() / chunk_size;
  const std::size_t last_chunk_size = data.size() % chunk_size;

  auto check_write = [&path, fp](std::size_t nr_written, std::size_t chunk_size){
    if (nr_written != chunk_size)
    {
      std::stringstream ss;
      ss << "Error while writing file " << path << '\n';
      std::fclose(fp);
      throw std::runtime_error(ss.str());
    }
  };

  for (std::size_t i = 0; i < nr_full_chunks; ++i)
  {
    const auto nr_written = std::fwrite(&data[i * chunk_size], std::size_t{1}, chunk_size, fp);
    check_write(nr_written, chunk_size);
  }

  const auto nr_written = std::fwrite(&data[nr_full_chunks * chunk_size], std::size_t{1}, last_chunk_size, fp);
  check_write(nr_written, last_chunk_size);

  std::fclose(fp);
}

}  // namespace sln
