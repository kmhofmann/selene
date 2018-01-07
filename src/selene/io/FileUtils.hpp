// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IO_FILE_UTILS_HPP
#define SELENE_IO_FILE_UTILS_HPP

/// @file

#include <cstdint>
#include <string>
#include <vector>

namespace sln {

std::vector<std::uint8_t> read_file_contents(const std::string& path);

void write_data_contents(const std::string& path, const std::vector<std::uint8_t>& data);

}  // namespace sln

#endif  // SELENE_IO_FILE_UTILS_HPP
