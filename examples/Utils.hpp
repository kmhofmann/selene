// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_EXAMPLES_UTILS_HPP
#define SELENE_EXAMPLES_UTILS_HPP

#include <selene/img/ImageDataToImage.hpp>
#include <selene/img/IO.hpp>

#include <selene/io/FileReader.hpp>

#include <boost/filesystem.hpp>

#include <cassert>
#include <iostream>
#include <string>

namespace sln_examples {

inline boost::filesystem::path full_data_path(const char* filename, const char* data_path = nullptr)
{
  const auto env_var = std::getenv("SELENE_DATA_PATH");
  boost::filesystem::path full_path;

  if (data_path)
  {
    full_path = boost::filesystem::path(data_path) / boost::filesystem::path(filename);
  }
  else if (env_var)
  {
    full_path = boost::filesystem::path(env_var) / boost::filesystem::path(filename);
  }
  else
  {
    full_path = boost::filesystem::path("../data") / boost::filesystem::path(filename);
  }

  if (!boost::filesystem::exists(full_path))
  {
    std::cout << "ERROR: File '" << full_path.string() << "' not found.\n";
    std::abort();
  }

  return full_path;
}

template <typename PixelType>
sln::Image<PixelType> read_example_image(const char* filename, const char* data_path = nullptr)
{
  // Get full path of example image
  const auto img_path = sln_examples::full_data_path(filename, data_path);
  std::cout << "Reading example image from '" << img_path.string() << "'\n";
  auto img_data = sln::read_image(sln::FileReader(img_path.string()));

  if (!img_data.is_valid())
  {
    std::cout << "ERROR: Image could not be read or decoded.\n";
    std::cout << "You might need to specify SELENE_DATA_PATH to the data directory as an environment variable.\n";
    std::abort();
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