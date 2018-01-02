// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IMAGE_DATA_STORAGE_HPP
#define SELENE_IMG_IMAGE_DATA_STORAGE_HPP

/// @file

namespace sln {

template <typename T>
class Image;

/** \brief Describes options for image data storage: Constant (view, non-owned), or Modifiable (either owned or
 * non-owned)
 *
 */
enum class ImageDataStorage
{
  Constant,
  Modifiable
};

}  // namespace sln

#endif  // SELENE_IMG_IMAGE_DATA_STORAGE_HPP
