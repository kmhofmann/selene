// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace selene {
namespace io {

std::vector<std::uint8_t> read_file_contents(const std::string& path);

} // namespace io
} // namespace selene
