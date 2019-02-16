// This file is part of the `Selene` library.
// Copyright 2017-2019 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#include <test/utils/Utils.hpp>

namespace sln_test {

sln_fs::path get_tmp_path()
{
  const auto tmp_path = sln_fs::temp_directory_path() / "selene_test";

  if (!sln_fs::exists(tmp_path))
  {
    sln_fs::create_directories(tmp_path);
  }

  return tmp_path;
}

sln_fs::path full_data_path(const sln_fs::path& filename)
{
  // First, check if the environment variable SELENE_DATA_PATH is set, and if so, use it.
  const auto env_var = std::getenv("SELENE_DATA_PATH");

  if (env_var)
  {
    return sln_fs::path(env_var) / filename;
  }

  const auto root_path = sln_fs::current_path().root_path();

  // Try to search upwards, to find a data/ directory.
  auto cur_path = sln_fs::current_path();
  while (cur_path != root_path)
  {
    const auto data_path = cur_path / "data";
    if (sln_fs::exists(data_path) && sln_fs::is_directory(data_path))
    {
      return data_path / filename;
    }
    cur_path = cur_path.parent_path();
  }

  // Try to find a selene/data/ directory just outside the current directory.
  const auto data_path = sln_fs::current_path().parent_path() / "selene" / "data";
  if (sln_fs::exists(data_path) && sln_fs::is_directory(data_path))
  {
    return data_path / filename;
  }

  // Give up and return something...
  return filename;
}

}  // namespace sln_test
