// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_EXAMPLES_UTILS_HPP
#define SELENE_EXAMPLES_UTILS_HPP

#include <selene/base/io/FileReader.hpp>

#include <selene/img/ImageDataToImage.hpp>
#include <selene/img_io/IO.hpp>

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace sln_examples {

inline void print_help_and_exit(const char* error_message,
                                const std::vector<std::filesystem::path>* paths_considered = nullptr)
{
  std::cout << "ERROR: " << error_message << "\n\n";
  std::cout << "You likely need to specify the correct path to the selene 'data/' directory.\n";
  std::cout << "There are two possible ways to accomplish this:\n";
  std::cout << "- Give the path as the first (and only) argument to the example program;\n";
  std::cout << "- Give the path as content of the environment variable SELENE_DATA_PATH.\n\n";

  if (paths_considered != nullptr)
  {
    std::cout << "The following paths were considered:\n";
    for (const auto& path : *paths_considered)
    {
      std::cout << "- " << path.string() << '\n';
    }
    std::cout << '\n';
  }

  std::exit(-1);
}

inline std::filesystem::path full_data_path(const char* filename, const char* data_path = nullptr)
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  std::filesystem::path full_path;
  std::vector<std::filesystem::path> paths_considered;

  if (data_path)
  {
    full_path = std::filesystem::path(data_path) / std::filesystem::path(filename);
    paths_considered.emplace_back(full_path);

    if (std::filesystem::exists(full_path))
    {
      return full_path;
    }
  }

  if (env_var)
  {
    full_path = std::filesystem::path(env_var) / std::filesystem::path(filename);
    paths_considered.emplace_back(full_path);

    if (std::filesystem::exists(full_path))
    {
      return full_path;
    }
  }

  full_path = std::filesystem::path("../data") / std::filesystem::path(filename);
  paths_considered.emplace_back(full_path);

  if (std::filesystem::exists(full_path))
  {
    return full_path;
  }

  full_path = std::filesystem::path("../../data") / std::filesystem::path(filename);
  paths_considered.emplace_back(full_path);

  if (std::filesystem::exists(full_path))
  {
    return full_path;
  }

  print_help_and_exit("File not found.", &paths_considered);
  return full_path;  // circumventing a compiler warning, but this is really unreachable code
}

template <typename PixelType>
sln::Image<PixelType> read_example_image(const char* filename, const char* data_path = nullptr)
{
  // Get full path of example image
  const auto img_path = sln_examples::full_data_path(filename, data_path);
  std::cout << "Reading example image from file '" << img_path.string() << "'\n";
  auto img_data = sln::read_image(sln::FileReader(img_path.string()));

  if (!img_data.is_valid())
  {
    print_help_and_exit("Image data could not be decoded.");
  }

  std::cout << "Decoded image data: (" << img_data.width() << " x " << img_data.height() << "), "
            << img_data.nr_channels() << " channels, " << img_data.nr_bytes_per_channel() << " byte(s) per channel\n";
  std::cout << "\n";

  const auto img = sln::to_image<PixelType>(std::move(img_data));
  assert(img.is_valid());
  return img;
}

}  // namespace

#endif  // SELENE_EXAMPLES_UTILS_HPP
