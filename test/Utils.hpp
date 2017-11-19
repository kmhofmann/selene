// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_TEST_UTILS_HPP
#define SELENE_TEST_UTILS_HPP

#include <boost/filesystem.hpp>

inline boost::filesystem::path get_tmp_path()
{
  namespace fs = boost::filesystem;
  const auto tmp_path = fs::temp_directory_path() / "selene_test/";

  if (!fs::exists(tmp_path))
  {
    fs::create_directories(tmp_path);
  }

  return tmp_path;
}

#endif // SELENE_TEST_UTILS_HPP