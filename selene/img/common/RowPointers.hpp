// This file is part of the `Selene` library.
// Copyright 2017-2018 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_COMMON_ROW_POINTERS_HPP
#define SELENE_IMG_COMMON_ROW_POINTERS_HPP

/// @file

#include <selene/img/common/Types.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

namespace sln {

using RowPointers = std::vector<std::uint8_t*>;
using ConstRowPointers = std::vector<const std::uint8_t*>;

/** \brief Extracts a list of consecutive pointers to each image row from an image.
 *
 * @tparam ImageType The image type.
 * @param img An image to extract the row pointers from.
 * @return List of row pointers.
 */
template <typename ImageType>
auto get_row_pointers(ImageType& img) -> std::conditional_t<ImageType::is_modifiable, RowPointers, ConstRowPointers>
{
  using RowPointerType = std::conditional_t<ImageType::is_modifiable, RowPointers, ConstRowPointers>;
  using size_type = typename RowPointerType::size_type;
  RowPointerType row_pointers(static_cast<size_type>(img.height()));

  for (PixelIndex y = 0_idx; y < img.height(); ++y)
  {
    row_pointers[static_cast<size_type>(y)] = img.byte_ptr(y);
  }

  return row_pointers;
}

/** \brief Extracts a list of consecutive pointers to each image row from an image.
 *
 * @tparam ImageType The image type.
 * @param img An image to extract the row pointers from.
 * @return List of row pointers.
 */
template <typename ImageType>
auto get_row_pointers(const ImageType& img) -> std::conditional_t<ImageType::is_modifiable && ImageType::is_view, RowPointers, ConstRowPointers>
{
  using RowPointerType = std::conditional_t<ImageType::is_modifiable && ImageType::is_view, RowPointers, ConstRowPointers>;
  using size_type = typename RowPointerType::size_type;
  RowPointerType row_pointers(static_cast<size_type>(img.height()));

  for (PixelIndex y = 0_idx; y < img.height(); ++y)
  {
    row_pointers[static_cast<size_type>(y)] = img.byte_ptr(y);
  }

  return row_pointers;
}

/** \brief Extracts a list of consecutive (constant) pointers to each image row from an image.
 *
 * @tparam ImageType The image type.
 * @param img An image to extract the row pointers from.
 * @return List of constant row pointers.
 */
template <typename ImageType>
auto get_const_row_pointers(const ImageType& img) -> ConstRowPointers
{
  using size_type = typename ConstRowPointers::size_type;
  ConstRowPointers row_pointers(static_cast<size_type>(img.height()));

  for (PixelIndex y = 0_idx; y < img.height(); ++y)
  {
    row_pointers[static_cast<size_type>(y)] = img.byte_ptr(y);
  }

  return row_pointers;
}

}  // namespace sln

#endif  // SELENE_IMG_COMMON_ROW_POINTERS_HPP
