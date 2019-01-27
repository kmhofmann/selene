// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <selene/base/_impl/Utils.hpp>
#include <selene/base/Assert.hpp>

#include <ctime>

namespace sln {
namespace impl {

std::string get_date_time_string()
{
  // Format: "YYYY:MM:DD HH:MM:SS". Length is 20 bytes (incl '\0').
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  char buf[20];
  [[maybe_unused]] const auto n = std::strftime(buf, 20, "%Y:%m:%d %H:%M:%S", &tm);
  SELENE_ASSERT(n == 19);
  return std::string{buf};
}

}  // namespace impl
}  // namespace sln