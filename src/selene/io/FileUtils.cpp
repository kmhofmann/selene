// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/io/FileUtils.hpp>

#include <array>
#include <cstdio>
#include <sstream>

namespace selene {
namespace io {

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
  std::array<std::uint8_t, buffer_size> buffer;
  auto fp = std::fopen(path.c_str(), "rb");

  if (fp == nullptr)
  {
    std::stringstream ss;
    ss << "Cannot open file " << path;
    throw std::runtime_error(ss.str());
  }

  std::vector<std::uint8_t> contents;
  while (!std::feof(fp))
  {
    auto nr_read = std::fread(buffer.data(), sizeof(std::uint8_t), buffer_size, fp);
    contents.insert(std::end(contents), buffer.data(), buffer.data() + nr_read);
  }

  return contents;
}


} // namespace io
} // namespace selene
