// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/io/FileUtils.hpp>

#include <array>
#include <cstdio>
#include <sstream>

namespace sln {

/** \brief Reads the binary contents of a file and returns them.
 *
 * \param path A string representing the path to the file that should be read.
 * \return An optional `std::vector<std::uint8_t>` with the contents of the file.
 *   `std::nullopt` will be returned, if the file can not be opened or read.
 */
std::optional<std::vector<std::uint8_t>> read_file_contents(const std::string& path)
{
  constexpr std::size_t buffer_size = 32768;
  std::array<std::uint8_t, buffer_size> buffer{};
  auto fp = std::fopen(path.c_str(), "rb");

  if (fp == nullptr)
  {
    return std::nullopt;
  }

  std::vector<std::uint8_t> contents;
  while (!std::feof(fp))
  {
    auto nr_read = std::fread(buffer.data(), sizeof(std::uint8_t), buffer_size, fp);
    contents.insert(std::end(contents), buffer.data(), buffer.data() + nr_read);
  }

  std::fclose(fp);
  return std::optional<std::vector<std::uint8_t>>{std::move(contents)};
}

/** \brief Writes the contents of `data` to a file.
 *
 * @param path A string representing the path to the file to be written.
 * @param data_ptr A pointer to the beginning of the data to be written.
 * @param data_len The length of the data in bytes.
 * @return True, if the writing operation succeeded; false otherwise.
 */
bool write_data_contents(const std::string& path, const std::uint8_t* data_ptr, std::size_t data_len)
{
  auto fp = std::fopen(path.c_str(), "wb");

  if (fp == nullptr)
  {
    return false;
  }

  constexpr std::size_t chunk_size = 65535;
  const std::size_t nr_full_chunks = data_len / chunk_size;
  const std::size_t last_chunk_size = data_len % chunk_size;

  for (std::size_t i = 0; i < nr_full_chunks; ++i)
  {
    const auto nr_written = std::fwrite(data_ptr + i * chunk_size, std::size_t{1}, chunk_size, fp);
    if (nr_written != chunk_size)
    {
      std::fclose(fp);
      return false;
    }
  }

  const auto nr_written = std::fwrite(data_ptr + nr_full_chunks * chunk_size, std::size_t{1}, last_chunk_size, fp);
  if (nr_written != chunk_size)
  {
    std::fclose(fp);
    return false;
  }

  std::fclose(fp);
  return true;
}

/** \brief Writes the contents of `data` to a file.
 *
 * @param path A string representing the path to the file to be written.
 * @param data_ptr A pointer to the beginning of the data to be written.
 * @param data_len The length of the data in bytes.
 */
bool write_data_contents(const std::string& path, const std::int8_t* data_ptr, std::size_t data_len)
{
  return write_data_contents(path, static_cast<const std::uint8_t*>(static_cast<const void*>(data_ptr)), data_len);
}

/** \brief Writes the contents of `data` to a file.
 *
 * @param path A string representing the path to the file to be written.
 * @param data_ptr A pointer to the beginning of the data to be written.
 * @param data_len The length of the data in bytes.
 */
bool write_data_contents(const std::string& path, const char* data_ptr, std::size_t data_len)
{
  return write_data_contents(path, static_cast<const std::uint8_t*>(static_cast<const void*>(data_ptr)), data_len);
}

/** \brief Writes the contents of `data` to a file.
 *
 * @param path A string representing the path to the file to be written.
 * @param data A `std::vector<std::uint8_t>` with the contents to be written.
 */
bool write_data_contents(const std::string& path, const std::vector<std::uint8_t>& data)
{
  return write_data_contents(path, data.data(), data.size());
}

}  // namespace sln
