// This file is part of the `Selene` library.
// Copyright 2017 Michael Hofmann (https://github.com/kmhofmann).
// Distributed under MIT license. See accompanying LICENSE file in the top-level directory.

#ifndef SELENE_IMG_IO_HPP
#define SELENE_IMG_IO_HPP

/// @file

#include <selene/img/ImageData.hpp>

namespace selene {
namespace img {

/** \brief Describes a supported image format for reading or writing.
 */
enum class ImageFormat
{
  JPEG,  ///< JPEG image format.
  PNG  ///< PNG image format.
};

template <typename SourceType>
ImageData read_image(SourceType& source, MessageLog* messages = nullptr);

template <typename SinkType>
bool write_image(const ImageData& img_data, ImageFormat format, SinkType& sink, MessageLog* messages = nullptr,
                 int jpeg_quality = 95);

} // namespace img
} // namespace selene

#endif // SELENE_IMG_IO_HPP
